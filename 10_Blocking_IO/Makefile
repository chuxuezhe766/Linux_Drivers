export ARCH=arm64
export CROSS_COMPILE=/home/kongchen/linux/RK3568/rk356x-linux/prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-
obj-m += blocking_io.o
KDIR := /home/kongchen/linux/RK3568/rk356x-linux/kernel
PWD ?= $(shell pwd)
all:
		make -C $(KDIR) M=$(PWD) modules
clean:
		make -C $(KDIR) M=$(PWD) clean