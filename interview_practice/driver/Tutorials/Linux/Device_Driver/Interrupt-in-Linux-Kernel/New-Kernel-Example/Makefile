obj-m += driver.o
 
KDIR = /lib/modules/$(shell uname -r)/build
#KDIR = /home/abhishekkumar/abhi/driver/linux-5.4.233

all:
	make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
	make -C $(KDIR)  M=$(shell pwd) clean
