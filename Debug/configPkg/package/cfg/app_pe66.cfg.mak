# invoke SourceDir generated makefile for app.pe66
app.pe66: .libraries,app.pe66
.libraries,app.pe66: package/cfg/app_pe66.xdl
	$(MAKE) -f /home/greg/workspace_v7/ti-rtos-uart-test/src/makefile.libs

clean::
	$(MAKE) -f /home/greg/workspace_v7/ti-rtos-uart-test/src/makefile.libs clean

