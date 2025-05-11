# TCS34725_DRIVER

- **NAME: Nguyen Thanh Danh studentID: 22146282**
- **NAME: Tran Xuan Hoang   studentID: 22146311**
- **NAME: Tran Anh Tri      studentID: 22146427**
- **NAME: Nguyen Huynh Anh  studentID: 22146263**

## OVERVIEW
TCS34725 RGB color sensor.
This driver provide function in user space to get data in kernel.  
Using i2c driver and register a i2c device tree 

## INSTALL
Register a driver will create a character device at /dev/tcs34725.
One more thing you have to do is register I2c device name "tcs34725@29" in I2C device tree.

Make sure the Makefile like that
'''bash
obj-m += TCS34725_Driver_ioctrl.o
KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(shell pwd) modules
clean: 
	make -C $(KDIR) M=$(shell pwd) clean
