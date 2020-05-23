obj-m   := dev_driver.o

KDIR    :=/work/achroimx_kernel
PWD     :=$(shell pwd)

all: driver

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

install:
	cp -a dev_driver.ko /nfsroot
	cp -a $(APP) /nfsroot

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf $(APP)
	rm -rf Module.symvers
	rm -rf modules.order
	rm -rf .tmp*
	rm -rf .fpga*
	rm -rf .*.cmd