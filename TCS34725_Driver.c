#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/delay.h>

#define DRIVER_NAME "tcs34725_driver"

#define TCS34725_COMMAND_BIT 0x80 // Next bit is register address
#define TCS34725_ENABLE 0x00 // Register enable, Reg = Register
#define TCS34725_CDATAL 0x14 // Reg clear low
#define TCS34725_RDATAL 0x16 // Reg red low
#define TCS34725_GDATAL 0x18 // Reg green low
#define TCS34725_BDATAL 0x1A // Reg blue low
#define TCS34725_ENABLE_PON 0x01
#define TCS34725_ENABLE_AEN 0x02

static struct i2c_client *tcs34725_client;

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

static int tcs34725_read_color_data(struct i2c_client *client)
{
    int c, r, g, b;

    // Data all color
    c = tcs34725_read_data(client, TCS34725_CDATAL); 

    // Data red color
    r = tcs34725_read_data(client, TCS34725_RDATAL); 

    // Data green color
    g = tcs34725_read_data(client, TCS34725_GDATAL);

    // Data blue color
    b = tcs34725_read_data(client, TCS34725_BDATAL);

    if (c < 0 || r < 0 || g < 0 || b < 0) {
        printk(KERN_ERR "Failed to read color data\n");
        return -EIO;
    }

    printk(KERN_INFO "TCS34725 Read Color Data Clear: %d, Red: %d, Green: %d, Blue: %d\n", c, r, g, b);
    return 0;
}

static int tcs34725_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret;
    tcs34725_client = client;

    // Enable sensor
    ret = i2c_smbus_write_byte_data(client, TCS34725_COMMAND_BIT | TCS34725_ENABLE, TCS34725_ENABLE_PON);
    // waiting TCS enable
    msleep(3); 
    if (ret < 0) {
        printk(KERN_ERR "Failed to enable TCS34725\n");
        return ret;
    }

    // Enable ADC
    ret = i2c_smbus_write_byte_data(client, TCS34725_COMMAND_BIT | TCS34725_ENABLE,TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
    // waiting ADC init
    msleep(45);  
    if (ret < 0) {
        printk(KERN_ERR "Failed to enable ADC TCS34725\n");
        return ret;
    }

    // Read data from TCS34725 sensor
    ret = tcs34725_read_color_data(client);
    if (ret < 0) {
        return ret;
    }

    printk(KERN_INFO "TCS34725 driver installed\n");
    return 0;
}

static void tcs34725_remove(struct i2c_client *client)
{
    printk(KERN_INFO "TCS34725 driver removed\n");

}

static const struct i2c_device_id tcs34725_id[] = {
    { "tcs34725", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, tcs34725_id);

static struct i2c_driver tcs34725_driver = {
    .driver = {
        .name   = DRIVER_NAME,
        .owner  = THIS_MODULE,
    },
    .probe      = tcs34725_probe,
    .remove     = tcs34725_remove,
    .id_table   = tcs34725_id,
};

static int __init tcs34725_init(void){
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
MODULE_DESCRIPTION("Driver TCS34725  I2C Client Kernel Raspberrypi");
