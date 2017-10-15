#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/tl/ti/syslink_2_21_01_05/packages;/home/tl/ti/bios_6_35_04_50/packages;/home/tl/ti/ipc_1_25_03_15/packages;/home/tl/ti/xdctools_3_25_03_72/packages;/home/tl/ti/xdais_7_21_01_07/packages
override XDCROOT = /home/tl/ti/xdctools_3_25_03_72
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/tl/ti/syslink_2_21_01_05/packages;/home/tl/ti/bios_6_35_04_50/packages;/home/tl/ti/ipc_1_25_03_15/packages;/home/tl/ti/xdctools_3_25_03_72/packages;/home/tl/ti/xdais_7_21_01_07/packages;/home/tl/ti/xdctools_3_25_03_72/packages;..
HOSTOS = Linux
endif
