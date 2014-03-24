SRCS := app.c
obj-m := vmshow.o

SYSCALL_ADDRESS = 0x$(subst R sys_call_table,,$(shell grep sys_call_table /boot/System.map-$(shell uname -r)))
CFLAGS_vmshow.o += -DSYSCALL_TABLE=$(SYSCALL_ADDRESS)

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

run:default app
	sudo insmod vmshow.ko
	./$(SRCS:.c=)
	sudo rmmod vmshow

$(SRCS:.c=):$(SRCS:.c=.o)

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
	$(RM) -rf app.o app *~
