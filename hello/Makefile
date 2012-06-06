obj-m += hello.o
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
#KERNELDIR ?= /lib/modules/3.2.0-23-generic-pae/build/
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules V=1

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

