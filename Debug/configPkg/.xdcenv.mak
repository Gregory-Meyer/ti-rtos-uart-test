#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/greg/ti/bios_6_52_00_12/packages;/home/greg/ti/pdk_k2l_4_0_9/packages;/home/greg/ti/dsplib_c66x_3_4_0_0/packages;/home/greg/ti/ccsv7/ccs_base
override XDCROOT = /home/greg/ti/xdctools_3_50_04_43_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/greg/ti/bios_6_52_00_12/packages;/home/greg/ti/pdk_k2l_4_0_9/packages;/home/greg/ti/dsplib_c66x_3_4_0_0/packages;/home/greg/ti/ccsv7/ccs_base;/home/greg/ti/xdctools_3_50_04_43_core/packages;..
HOSTOS = Linux
endif
