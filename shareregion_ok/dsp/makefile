MCSDK                    = /home/tl/ti
CCS                      = /home/tl/ti
STARTWARE_INSTALL_DIR    = /home/tl/OMAPL138_StarterWare_1_10_04_01

SYSLINK_INSTALL_DIR      = $(MCSDK)/syslink_2_21_01_05
IPC_INSTALL_DIR          = $(MCSDK)/ipc_1_25_03_15
DSPLIB_INSTALL_DIR       = $(MCSDK)/dsplib_c674x_3_1_1_1
BIOS_INSTALL_DIR         = $(CCS)/bios_6_35_04_50
MATHLIB_INSTALL_DIR      = $(MCSDK)/mathlib_c674x_3_0_2_0
XDC_INSTALL_DIR          = $(CCS)/xdctools_3_25_03_72
CGT_C674_ELF_INSTALL_DIR = $(CCS)/ccsv5/tools/compiler/c6000_7.4.4

srcs = main_dsp.c syslink_service.c server.c syslink.c resource_sync.c gray_bmp_rotater.c

objs = $(addprefix bin/$(PROFILE)/obj/,$(patsubst %.c,%.oe674,$(srcs)))
libs = configuro/linker.cmd
dsp_config=dsp.cfg

all: configuro/linker.cmd
	make PROFILE=debug server_dsp.x
	#make PROFILE=release server_dsp.x

server_dsp.x: bin/$(PROFILE)/server_dsp.xe674
bin/$(PROFILE)/server_dsp.xe674: $(objs) $(libs)
	@echo "#"
	@echo "# Making $@ ..."
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

bin/$(PROFILE)/obj/%.oe674: %.h
bin/$(PROFILE)/obj/%.oe674: %.c log.h ../shared/protocol.h ../shared/sys_config.h
	@echo "#"
	@echo "# Making $@ ..."
	$(CC) $(CPPFLAGS) $(CFLAGS) --output_file=$@ -fc $<

configuro/linker.cmd: $(dsp_config) ../shared/config.bld
	@echo "#"
	@echo "# Making $@ ..."
	$(XDC_INSTALL_DIR)/xs --xdcpath="$(subst +,;,$(PKGPATH))"       \
            xdc.tools.configuro -o configuro                        \
            -t ti.targets.elf.C674 -c $(CGT_C674_ELF_INSTALL_DIR)   \
            -p ti.platforms.evmOMAPL138:dsp -b ../shared/config.bld \
            -r release $(dsp_config) 

clean::
	rm -rf configuro bin

PKGPATH := $(SYSLINK_INSTALL_DIR)/packages
PKGPATH := $(PKGPATH)+$(BIOS_INSTALL_DIR)/packages
PKGPATH := $(PKGPATH)+$(IPC_INSTALL_DIR)/packages
PKGPATH := $(PKGPATH)+$(XDC_INSTALL_DIR)/packages
PKGPATH := $(PKGPATH)+$(MATHLIB_INSTALL_DIR)/packages

CGTOOLS = $(CGT_C674_ELF_INSTALL_DIR)

CC = $(CGTOOLS)/bin/cl6x -c --gcc # gcc extensions support
AR = $(CGTOOLS)/bin/ar6x rq
LD = $(CGTOOLS)/bin/lnk6x --abi=eabi
ST = $(CGTOOLS)/bin/strip6x

CPPFLAGS =
CFLAGS = -qq $(CCPROFILE_$(PROFILE)) $(COMPILER_OPTS)     \
		 -I$(MATHLIB_INSTALL_DIR)/packages                \
		 -I$(MATHLIB_INSTALL_DIR)/inc                     \
		 -I$(STARTWARE_INSTALL_DIR)/include               \
		 -I$(STARTWARE_INSTALL_DIR)/include/hw            

LDFLAGS = -w -q -c -m $(@D)/obj/$(@F).map

LDLIBS  = -l $(CGTOOLS)/lib/rts6740_elf.lib												    \
          -l $(MATHLIB_INSTALL_DIR)/lib/mathlib.ae674									    \
		  -l $(STARTWARE_INSTALL_DIR)/binary/c674x/cgt/utils/Release/utils.lib              \
		  -l Platform.lib                                                                   \
          -l $(STARTWARE_INSTALL_DIR)/binary/c674x/cgt/omapl138/drivers/Release/drivers.lib \
		  -l $(DSPLIB_INSTALL_DIR)/lib/dsplib.ae674 

CCPROFILE_debug = -D_DEBUG_=1 --symdebug:dwarf
CCPROFILE_release = -O2
COMPILER_OPTS = $(shell cat configuro/compiler.opt)

ifneq (clean,$(MAKECMDGOALS))
ifneq (,$(PROFILE))
ifeq (,$(wildcard bin/$(PROFILE)/obj))
    $(shell mkdir -p bin/$(PROFILE)/obj)
endif
endif
endif
