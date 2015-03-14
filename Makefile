################################################################
#
#  This confidential and  proprietary  software may be used only
#  as authorised  by  a licensing  agreement  from  ARM  Limited
#
#             (C) COPYRIGHT 2013-2015 ARM Limited
#                    ALL RIGHTS RESERVED
#
#   The entire notice above must be reproduced on all authorised
#   copies and copies  may only be made to the  extent permitted
#   by a licensing agreement from ARM Limited.
#
################################################################

PREFIX:=arm-none-eabi-
PROJECT:=uvisor-helloworld
ARCH:=MK64F
CPU:=$(ARCH)N1M0XXX12
TARGET:=frdm-k64f-gcc

#
# flash firmware image using segger JLink
# https://www.segger.com/jlink-software.html
#
JLINK:=$(SEGGER)JLinkExe
JLINK_VIEWER_MASK:=0x7
JLINK_PARAM:=-Device $(CPU) -if SWD
AUXHFRCO_FREQ:=0
JLINK_SWO_PARAM:=

#
#
# GDB configuration
GDB:=$(PREFIX)gdb
DEBUG_HOST:=localhost:2331

#
# derived variables
#
TARGET:=./build/$(TARGET)/source/$(PROJECT)
TARGET_BIN:=$(TARGET).bin
JLINK_SERVER:=$(SEGGER)JLinkGDBServer
JLINK_VIEWER:=$(SEGGER)JLinkSWOViewer

#
# add inline files for debugging / flashing
#
include Makefile.scripts

# check if uvisor symbol file can be read - add to symbols GDB config if present
GDB_DEBUG_SYMBOLS_FILE:=$(wildcard ../uvisor*/k64f/uvisor/k64f_uvisor.elf)
ifneq ("$(GDB_DEBUG_SYMBOLS_FILE)","")
	GDB_DEBUG_UVISOR:=add-symbol-file $(GDB_DEBUG_SYMBOLS_FILE) uvisor_init
endif

#
# build targets
#

.PHONY: all setup firmware relase flash erase reset gdb gdbserver swo clean

all: release

setup: clean
	yotta target frdm-k64f-gcc

firmware: $(TARGET_BIN)

release: setup
	yotta build -r
	@$(PREFIX)size $(TARGET)

flash: firmware
	@echo "$$__SCRIPT_FLASH" | $(JLINK) $(JLINK_PARAM)
	@$(PREFIX)size $(TARGET)

erase:
	@echo "$$__SCRIPT_ERASE" | $(JLINK) $(JLINK_PARAM)

reset:
	@echo "$$__SCRIPT_RESET" | $(JLINK) $(JLINK_PARAM)

gdb.script: firmware $(TARGET)
	@echo "$$__SCRIPT_GDB" > $@

gdb: gdb.script
	$(GDB) -x $<

gdbtui: gdb.script
	$(GDB) -tui -x $<

gdbserver:
	$(JLINK_SERVER) $(JLINK_PARAM) $(APP_JLINK_PARAM)

swo: reset
	$(JLINK_VIEWER) $(JLINK_PARAM) -itmmask $(JLINK_VIEWER_MASK) $(JLINK_SWO_PARAM)

$(TARGET_BIN):
	yotta build -d
	@$(PREFIX)size $(TARGET)

clean:
	rm -f gdb.script
	yotta clean
