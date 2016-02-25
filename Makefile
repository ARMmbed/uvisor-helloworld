###########################################################################
#
#  Copyright (c) 2013-2015, ARM Limited, All Rights Reserved
#  SPDX-License-Identifier: Apache-2.0
#
#  Licensed under the Apache License, Version 2.0 (the "License"); you may
#  not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
#  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

PREFIX:=arm-none-eabi-
PROJECT:=uvisor-helloworld

ifeq ("$(ARCH)","MK64F")
	CPU:=$(ARCH)N1M0XXX12
ifeq ("$(TOOL)","armcc")
	YT_TARGET:=frdm-k64f-armcc
else
	YT_TARGET:=frdm-k64f-gcc
endif
	GDB_TARGET:=k64f
else ifeq ("$(ARCH)","EFM32")
	CPU:=$(ARCH)$(FAMILY)$(DEVICE)
	GDB_TARGET:=efm32
	ifeq ("$(FAMILY)","GG")
		YT_TARGET:=efm32gg-stk-gcc
	endif
	ifeq ("$(FAMILY)","LG")
		YT_TARGET:=efm32lg-stk-gcc
	endif
	ifeq ("$(FAMILY)","WG")
		YT_TARGET:=efm32wg-stk-gcc
	endif
	ifeq ("$(FAMILY)","PG1B")
		YT_TARGET:=efm32pg-stk-gcc
	endif
else
	ARCH:=STM32F4
	CPU:=$(ARCH)29ZI
	YT_TARGET:=stm32f429i-disco-gcc
	GDB_TARGET:=stm32f4
endif

#
# flash firmware image using segger JLink
# https://www.segger.com/jlink-software.html
#
JLINK:=$(SEGGER)JLinkExe
JLINK_VIEWER_MASK:=0x7
JLINK_PARAM:=-Device $(CPU) -if SWD
AUXHFRCO_FREQ:=0

#
#
# GDB configuration
GDB:=$(PREFIX)gdb
DEBUG_HOST:=localhost:2331
OBJDUMP:=$(PREFIX)objdump

#
# derived variables
#
TARGET:=./build/$(YT_TARGET)/source/$(PROJECT)
TARGET_BIN:=$(TARGET).bin
JLINK_SERVER:=$(SEGGER)JLinkGDBServer

#
# add inline files for debugging / flashing
#
include Makefile.scripts

# check if uvisor symbol file can be read - add to symbols GDB config if present
GDB_DEBUG_SYMBOLS_FILE:=$(wildcard ../uvisor*/$(GDB_TARGET)/uvisor/$(GDB_TARGET)_uvisor.elf)
ifneq ("$(GDB_DEBUG_SYMBOLS_FILE)","")
	GDB_DEBUG_UVISOR:=add-symbol-file $(GDB_DEBUG_SYMBOLS_FILE) uvisor_init
endif

#
# build targets
#

.PHONY: all setup firmware relase flash erase reset gdb.script gdb gdbtui gdbserver clean

all: debug gdbtui

setup:
	yotta target $(YT_TARGET)

debug: setup
	yotta build -d -- -j8
	$(OBJDUMP) -d $(TARGET) > $(PROJECT).asm
	@$(PREFIX)size $(TARGET)

release: setup
	yotta build -r -- -j8
	$(OBJDUMP) -d $(TARGET) > $(PROJECT).asm
	@$(PREFIX)size $(TARGET)

flash:
	@echo "$$__SCRIPT_FLASH" | $(JLINK) $(JLINK_PARAM)
	@$(PREFIX)size $(TARGET)

erase:
	@echo "$$__SCRIPT_ERASE" | $(JLINK) $(JLINK_PARAM)

reset:
	@echo "$$__SCRIPT_RESET" | $(JLINK) $(JLINK_PARAM)

jlink:
	$(JLINK) $(JLINK_PARAM)

gdb.script:
	@echo "$$__SCRIPT_GDB" > $@

gdb: gdb.script
	$(GDB) -x $<

gdbtui: gdb.script
	$(GDB) -tui -x $<

gdbserver:
	$(JLINK_SERVER) $(JLINK_PARAM) $(APP_JLINK_PARAM)

$(TARGET_BIN): debug

clean: setup
	rm -f gdb.script $(PROJECT).asm
	yotta clean
