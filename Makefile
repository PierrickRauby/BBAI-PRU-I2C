COMMON:=/var/lib/cloud9/common
PRU_CGT:=/usr/share/ti/cgt-pru
PRU_SUPPORT:=/usr/lib/ti/pru-software-support-package
PRU_STARTERWARE:=/usr/share/ti/starterware
C6X_CGT:=/usr/share/ti/cgt-c6x
TIDL_API_DIR:=/usr/share/ti/tidl/tidl_api
GEN_DIR:=$(abspath $(lastword $(MAKEFILE_LIST)/..))/tmp
MODEL:=$(shell cat /proc/device-tree/model | sed 's/ /_/g' | tr -d '\000')

TARGET := $(TARGET)
OBJ := $(TARGET).o  $(GEN_DIR)/am572x_pru_i2c_driver.pru1_1.o

ifeq ($(TARGET),)
$(warning "Warning no TARGET defined, target set to main")
TARGET=main
OBJ := $(TARGET).o $(GEN_DIR)/am572x_pru_i2c_driver.pru1_1.o
endif



CHIP=am57xx
CHIP_REV=am572x_2_0
PRU_DIR=/sys/class/remoteproc/remoteproc5
PRUN=1_1
PROC=pru
EXE=.out


LD=lnkpru -o
LDFLAGS=--reread_libs --warn_sections --stack_size=0x100 --heap_size=0x100 -m $(basename $(1)).map \
	-i$(PRU_CGT)/lib -i$(PRU_CGT)/include $(COMMON)/$(CHIP)_$(PROC).cmd --library=libc.a \
	--library=$(PRU_SUPPORT)/lib/rpmsg_lib.lib
CC=clpru -fe
CFLAGS= --include_path=./include --include_path=$(COMMON) --include_path=$(PRU_SUPPORT)/include \
	--include_path=$(PRU_SUPPORT)/include/$(CHIP_REV) \
	--include_path=$(PRU_STARTERWARE)/include --include_path=$(PRU_STARTERWARE)/include/hw \
	--include_path=$(PRU_CGT)/include -DCHIP=$(CHIP) -DCHIP_IS_$(CHIP) -DMODEL=$(MODEL) -DPROC=$(PROC) -DPRUN=$(PRUN) \
	-v3 -O2 --printf_support=minimal --display_error_number --endian=little --hardware_mac=on \
	--obj_directory=$(GEN_DIR) --pp_directory=$(GEN_DIR) --asm_directory=$(GEN_DIR) -ppd -ppa --asm_listing \
	--c_src_interlist

all: stop compile place start
	@echo '---> DONE: PRU$(PRUN) should be up and running' 


stop:
	@echo "-    Stopping PRU $(PRUN)"
	@echo stop > $(PRU_DIR)/state || echo Cannot stop $(PRUN)

start:
	@echo "-    Starting PRU $(PRUN)"
	@echo start > $(PRU_DIR)/state


place: 
	@echo '-    Copying firmware file $(GEN_DIR)/$(TARGET)$(EXE) to /lib/firmware/$(CHIP)-pru$(PRUN)-fw'
	@cp $(GEN_DIR)/$(TARGET)$(EXE) /lib/firmware/$(CHIP)-pru$(PRUN)-fw

compile: $(GEN_DIR)/$(TARGET)$(EXE)


$(GEN_DIR)/$(TARGET)$(EXE): $(GEN_DIR)/$(OBJ)
	@echo "linking....$(GEN_DIR)/$(OBJ)"
	@$(LD) $@ $^ $(LDFLAGS) 


$%.o:%.c
	@echo "compiled $(GEN_DIR)/%.o"
	@mkdir -p $(GEN_DIR)
	@echo 'CC	$^'
	@$(CC) --output_file=$@ $^ $(CFLAGS)

$(GEN_DIR)/%.o: %.c
	@echo "compiled $(GEN_DIR)/%.o"
	@mkdir -p $(GEN_DIR)
	@echo 'CC	$^'
	@$(CC) $@ $^ $(CFLAGS)


clean:
	@echo 'CLEAN'
	@rm -rf $(GEN_DIR)

