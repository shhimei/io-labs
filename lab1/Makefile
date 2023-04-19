obj-m += char_mod.o

PWD := $(shell pwd)
MOD := char_mod.ko

all: build load

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o user user.c

clean: remove
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	${RM} user

load:
	insmod $(MOD)

remove:
	rmmod $(MOD)

