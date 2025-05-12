# TCS34725_DRIVER

- **NAME: Nguyen Thanh Danh**
- **StudentID: 22146282**
- **NAME: Tran Xuan Hoang**
- **StudentID: 22146311**
- **NAME: Tran Anh Tri**
- **StudentID: 22146427**
- **NAME: Nguyen Huynh Anh**
- **StudentID: 22146263**

## OVERVIEW
TCS34725 RGB color sensor.
This driver ioctrl provide function in user space to get data in kernel.  
Using i2c driver and register a i2c device tree. 

## INSTALL
Register a driver will create a character device at /dev/tcs34725.
One more thing you have to do is register I2C device name "tcs34725@29" in I2C device tree.
### DEVICE CLIENT
1. To register a device in device tree, let change directory to the boot
Change a dtb file to load a file when Raspberry starting and boot a I2C devide add a client.
I use a Raspberry Pi 4B so the dtb file is bcm2711-rpi-4-b.dtb.
Dtb is Device tree binray, so transfer it into dts to edit.
```bash
	cd /boot
	dtc -I dtb -O dts -o bcm2711-rpi-4-b.dts bcm2711-rpi-4-b.dtb
	sudo geany	
```
2. After that, edit bcm2711-rpi-4-b.dts with the sudo permission
In that case, i use geany to find a character easy.
The I2C we use in Raspberry is I2C1, then we find a code just like. 
```dts
	&i2c1 {
        	compatible = "<name>";
        	status	= "disable";
	};
```
3. Add the client using I2C, repeat if u have any driver running base on I2C in the future
```dts
	&i2c1 {
        	compatible = "<name>";
        	status	= "okay";
		tcs34725@29 {
        		compatible = "tcs34725";
        		reg = <0x29>;
	};
```
Remember to change status to okay.
reg is a address of sensor in I2C protocol.
compatible to set Raspberry recognize sensor when it binding in to I2C bus
4. Then save a dts file, change it back to dtb file and reboot a Raspberry
```bash
	dtc -I dts -O dtb -o bcm2711-rpi-4-b.dtb bcm2711-rpi-4-b.dts
	sudo reboot
```
### INSTALL DRIVER
1. Make sure the Makefile have a correct driver name you want to compile in obj-m += name.o

```bash
	obj-m += TCS34725_Driver_ioctrl.o
	KDIR = /lib/modules/$(shell uname -r)/build
	
	all:
		make -C $(KDIR) M=$(shell pwd) modules
	clean: 
		make -C $(KDIR) M=$(shell pwd) clean
```

<uname -r> give a kernel name is running, and certainly it have a build file
If it doesn't have, you should change a kernel version have a build file.

2. Build a .ko file using cmd
```bash
	make
```

3.Install a driver
```bash
	sudo insmod TCS34725_Driver_ioctrl.ko
```
In that case TCS34725_Driver_ioctrl are set in Makefile.
4. You can check a notice in kernel log file
```bash
	dmesg
```
5 If you want to uninstall driver run this cmd
```bash
	sudo rmmod TCS34725_Driver_ioctrl.ko
```
6. Then remove all file compile
```bash
	make clean
```
### FUNCTION PROVIDE
CHARACTER DEVICE API
1. ```open(), close()```
   Standard open() and close() operations on /dev/tcs34725 are supported.

2. ```read()```
   Currently not implemented. Use ioctl() to get color data.

3. ```c
   ioctl()
   #define TCS34725_IOC_MAGIC 't'
   #define TCS34725_GET_CLEAR   _IOR(TCS34725_IOC_MAGIC, 1, int)
   #define TCS34725_GET_RED     _IOR(TCS34725_IOC_MAGIC, 2, int)
   #define TCS34725_GET_GREEN   _IOR(TCS34725_IOC_MAGIC, 3, int)
   #define TCS34725_GET_BLUE    _IOR(TCS34725_IOC_MAGIC, 4, int)
   #define TCS34725_IOCTL_SET_GAIN    _IOW(TCS34725_IOCTL_MAGIC, 5, int)
   #define TCS34725_IOCTL_SET_ATIME   _IOW(TCS34725_IOCTL_MAGIC, 6, int)
   ```
Example:
```c
	int fd;
	fd = open(DEVICE_NAME, O_RDWR);
	if (fd < 0) {
        	perror("Failed to open the device");
        	return errno;
	}
       	if (ioctl(fd, TCS34725_IOCTL_READ_C, &color_data) < 0) {
        	perror("Failed to read Clear Color Data");    
        	close(fd);
        	return errno;
    	} 
    	else {
        	printf("Read Clear Color Data: %d ", color_data);
    	}
   	close(fd);
```
TCS34725_GET_CLEAR is get the intensity all color. 
TCS34725_GET_RED is get the intensity of RED.
TCS34725_GET_GREEN is get the intensity of GREEN.
TCS34725_GET_BLUE is get the intensity of BLUE.
Every color in range of 16 bit data.
TCS34725_IOCTL_SET_GAIN is set the sensitivity of sensor.
Gain is setting the sensor sensitivity:  0x00, 0x01, 0x02, 0x03 equal to 1x 4x 16x 60x.
TCS34725_IOCTL_SET_ATIM is set the integrate time for sensor
integration time = (256 - atime ) *2.4, short time get data quick, long time more accurate.
## TEST DRIVER IN KERNEL
1. The Makefile change a obj-m to driver file
   ```
	obj-m += TCS34725_driver.o
	
	all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
	clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```
Then do the same
2. Build a .ko file using cmd
```bash
	make
```

3.Install a driver
```bash
	sudo insmod TCS34725_Driver_ioctrl.ko
```
In that case TCS34725_Driver_ioctrl are set in Makefile.
4. You can check a notice in kernel log file
```bash
	dmesg
```
If running, the log file send a data get 1 time in sensor.
If no data is sending, check the I2C recognize a sensor before.
```bash
	i2cdetect -y 1
```
5 If you want to uninstall driver run this cmd
```bash
	sudo rmmod TCS34725_Driver_ioctrl.ko
```
6. Then remove all file compile
```bash
	make clean
```
