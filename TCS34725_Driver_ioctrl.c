#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/delay.h>

#define DRIVER_NAME "tcs34725_driver"
#define CLASS_NAME "tcs34725"
#define DEVICE_NAME "tcs34725"

#define TCS34725_COMMAND_BIT 0x80 // Next bit is register address
#define TCS34725_ENABLE 0x00 // Register enable, Reg = Register
#define TCS34725_CDATAL 0x14 // Reg clear low
#define TCS34725_RDATAL 0x16 // Reg red low
#define TCS34725_GDATAL 0x18 // Reg green low
#define TCS34725_BDATAL 0x1A // Reg blue low
#define TCS34725_ENABLE_PON 0x01
#define TCS34725_ENABLE_AEN 0x02

#define TCS34725_IOCTL_MAGIC 't'
#define TCS34725_IOCTL_READ_C _IOR(TCS34725_IOCTL_MAGIC, 1, int)
#define TCS34725_IOCTL_READ_R _IOR(TCS34725_IOCTL_MAGIC, 2, int)
#define TCS34725_IOCTL_READ_G _IOR(TCS34725_IOCTL_MAGIC, 3, int)
#define TCS34725_IOCTL_READ_B _IOR(TCS34725_IOCTL_MAGIC, 4, int)
#define TCS34725_IOCTL_SET_GAIN _IOW(TCS34725_IOCTL_MAGIC, 5, int)
#define TCS34725_IOCTL_SET_ATIME _IOW(TCS34725_IOCTL_MAGIC, 6, int)

static struct i2c_client *tcs34725_client;
static struct class* tcs34725_class = NULL;
static struct device* tcs34725_device = NULL;
static int major_number;

static int tcs34725_read_data(struct i2c_client *client, u8 reg)
{
    int low, high;

    // Read low byte data
    low = i2c_smbus_read_byte_data(client, TCS34725_COMMAND_BIT | reg);
    if (low < 0) return low;

    // Read high byte data
    high = i2c_smbus_read_byte_data(client, TCS34725_COMMAND_BIT | (reg + 1));
    if (high < 0) return high;

    // Combine and give back
    return (high << 8) | low;
}

static long tcs34725_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int data;
    int ret = 0;

    switch (cmd) {
        case TCS34725_IOCTL_READ_C:
            data = tcs34725_read_data(tcs34725_client, TCS34725_CDATAL);
            break;
        case TCS34725_IOCTL_READ_R:
            data = tcs34725_read_data(tcs34725_client, TCS34725_RDATAL);
            break;
        case TCS34725_IOCTL_READ_G:
            data = tcs34725_read_data(tcs34725_client, TCS34725_GDATAL);
            break;
        case TCS34725_IOCTL_READ_B:
            data = tcs34725_read_data(tcs34725_client, TCS34725_BDATAL);
            break;
        case TCS34725_IOCTL_SET_GAIN:
            if (copy_from_user(&data, (int __user *)arg, sizeof(data)))
                return -EFAULT;
            ret = i2c_smbus_write_byte_data(tcs34725_client, TCS34725_COMMAND_BIT | 0x0F, data);
            break;
        case TCS34725_IOCTL_SET_ATIME:
            if (copy_from_user(&data, (int __user *)arg, sizeof(data)))
                return -EFAULT;
            ret = i2c_smbus_write_byte_data(tcs34725_client, TCS34725_COMMAND_BIT | 0x01, data);
            break;
        default:
            return -EINVAL;
    }

    // Copy data form kernel to user space
    if (copy_to_user((int __user *)arg, &data, sizeof(data))) {
        ret = -EFAULT;
    }

    return ret;
}

static int tcs34725_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "TCS34725 device opened\n");
    return 0;
}

static int tcs34725_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "TCS34725 device closed\n");
    return 0;
}

static struct file_operations fops = {
    .open = tcs34725_open,
    .unlocked_ioctl = tcs34725_ioctl,
    .release = tcs34725_release,
};

static int tcs34725_probe(struct i2c_client *client, const struct i2c_device_id *id)
{

    tcs34725_client = client;

    // Create a char device
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ERR "Failed to register a major number\n");
        return major_number;
    }
    // Create a class 
    tcs34725_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(tcs34725_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ERR "Failed to register device class\n");
        return PTR_ERR(tcs34725_class);
    }
    // Create device for user
    tcs34725_device = device_create(tcs34725_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(tcs34725_device)) {
        class_destroy(tcs34725_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ERR "Failed to create the device\n");
        return PTR_ERR(tcs34725_device);
    }
    // Enable sensor
    if(i2c_smbus_write_byte_data(client, TCS34725_COMMAND_BIT | TCS34725_ENABLE, TCS34725_ENABLE_PON) < 0){
        printk(KERN_ERR "Failed to power TCS34725\n");
        return -EIO;
    }
    // Enable ADC
    if (i2c_smbus_write_byte_data(client, TCS34725_COMMAND_BIT | TCS34725_ENABLE,TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN) < 0) {
        printk(KERN_ERR "Failed to enable ADC TCS34725\n");
        return -EIO;
    }

    msleep(700);

    printk(KERN_INFO "TCS34725 driver installed\n");
    return 0;
}

static void tcs34725_remove(struct i2c_client *client)
{
    device_destroy(tcs34725_class, MKDEV(major_number, 0));
    class_unregister(tcs34725_class);
    class_destroy(tcs34725_class);
    unregister_chrdev(major_number, DEVICE_NAME);

    printk(KERN_INFO "TCS34725 driver removed\n");

}

static const struct of_device_id tcs34725_of_match[] = {
    { .compatible = "ams,tcs34725", },
    { },
};
MODULE_DEVICE_TABLE(of, tcs34725_of_match);

static struct i2c_driver tcs34725_driver = {
    .driver = {
        .name   = DRIVER_NAME,
        .owner  = THIS_MODULE,
        .of_match_table = of_match_ptr(tcs34725_of_match),
    },
    .probe      = tcs34725_probe,
    .remove     = tcs34725_remove,
};

static int __init tcs34725_init(void){\
	printk(KERN_INFO "Initializing TCS34725 driver\n");
	return i2c_add_driver(&tcs34725_driver);

}

static void __exit tcs34725_exit(void){
	printk(KERN_INFO "Exiting TCS34725 driver\n");
    i2c_del_driver(&tcs34725_driver);
}

module_init(tcs34725_init);
module_exit(tcs34725_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Danh-Hoang-Tri-Anh");
MODULE_DESCRIPTION("Driver TCS34725 communicate to user space Raspberrypi");
