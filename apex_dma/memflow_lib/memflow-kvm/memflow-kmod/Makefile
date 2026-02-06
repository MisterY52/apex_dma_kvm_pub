obj-y += memflow-kmod/

MCFLAGS += -O3
ccflags-y += ${MCFLAGS}
CC += ${MCFLAGS}

ifndef KERNELDIR
	KDIR := /lib/modules/$(shell uname -r)/build
else
	KDIR := $(KERNELDIR)
endif

KOUTPUT := $(PWD)

all:
	@echo "$(KOUTPUT)"
	make -C $(KDIR) M=$(KOUTPUT) src=$(PWD) modules

clean:
	make -C $(KDIR) M=$(KOUTPUT) src=$(PWD) clean
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
