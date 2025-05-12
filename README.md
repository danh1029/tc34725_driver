# TCS34725_DRIVER

- **NAME: Nguyen Thanh Danh studentID: 22146282**
- **NAME: Tran Xuan Hoang   studentID: 22146311**
- **NAME: Tran Anh Tri      studentID: 22146427**
- **NAME: Nguyen Huynh Anh  studentID: 22146263**

## OVERVIEW
TCS34725 RGB color sensor.
This driver ioctrl provide function in user space to get data in kernel.  
Using i2c driver and register a i2c device tree. 

## INSTALL
Register a driver will create a character device at /dev/tcs34725.
One more thing you have to do is register I2C device name "tcs34725@29" in I2C device tree.

1. To register a device in device tree, let change directory to the boot
I use a Raspberry Pi 4B so the dtb file is bcm2711-rpi-4-b.dtb.
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
Remember to change status to okay.
```dts
	&i2c1 {
        	compatible = "<name>";
        	status	= "okay";
		tcs34725@29 {
        		compatible = "tcs34725";
        		reg = <0x29>;
	};
```
4. Then save a dts file, change it back to dtb file and reboot a Raspberry
```bash
	dtc -I dts -O dtb -o bcm2711-rpi-4-b.dtb bcm2711-rpi-4-b.dts
	sudo reboot
```

1. Make sure the Makefile have a correct driver name you want to compile in obj-m += name.o

```bash
obj-m += TCS34725_Driver_ioctrl.o
KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(shell pwd) modules
clean: 
	make -C $(KDIR) M=$(shell pwd) clean
```

<unme -r> give a kernel name is running, and certainly it have a build file
If it doesn't have, you should change a kernel version have a build file.

2. Build a .ko file using cmd
```bash
	make
```

3.Install a driver
```bash
	sudo insmod TCS34725_Driver_ioctrl.ko
```
In that case TCS34725_Driver_ioctrl are set in Makefile
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
