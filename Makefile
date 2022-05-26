ifneq ($(KERNELRELEASE),)
obj-m := test_receive.o 
else
KDIR :=/lib/modules/$(shell uname -r)/build
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -f *.ko *.o *.mod.c *.symvers *.order
endif
