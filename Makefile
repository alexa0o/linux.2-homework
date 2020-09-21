obj-m += phonebook_module.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M+$(PWD) clean
test:
	sudo dmesg -C
	sudo insmod phonebook_module.ko
	dmesg
