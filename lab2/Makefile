ifeq ($(KERNELRELEASE),)
    PWD := $(shell pwd)
all: build

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

else
    obj-m := block_drv.o
endif