obj-m += TCS34725_Driver_ioctrl.o
KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(shell pwd) modules
clean: 
	make -C $(KDIR) M=$(shell pwd) clean