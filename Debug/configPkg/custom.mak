## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,e66 linker.cmd package/cfg/app_pe66.oe66

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/app_pe66.xdl
	$(SED) 's"^\"\(package/cfg/app_pe66cfg.cmd\)\"$""\"/home/greg/workspace_v7/ti-rtos-uart-test/Debug/configPkg/\1\""' package/cfg/app_pe66.xdl > $@
	-$(SETDATE) -r:max package/cfg/app_pe66.h compiler.opt compiler.opt.defs
