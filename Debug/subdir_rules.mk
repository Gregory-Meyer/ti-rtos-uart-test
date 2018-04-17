################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
build-1049211231:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-1049211231-inproc

build-1049211231-inproc: ../app.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"/home/greg/ti/xdctools_3_50_04_43_core/xs" --xdcpath="/home/greg/ti/bios_6_52_00_12/packages;/home/greg/ti/pdk_k2l_4_0_9/packages;/home/greg/ti/dsplib_c66x_3_4_0_0/packages;/home/greg/ti/mathlib_rts_c66x_3_1_2_1/packages;/home/greg/ti/ccsv7/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.elf.C66 -p ti.platforms.evmC66AK2E -r release -c "/home/greg/ti/ti-cgt-c6000_8.2.2" --compileOptions "-g --optimize_with_debug" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-1049211231 ../app.cfg
configPkg/compiler.opt: build-1049211231
configPkg/: build-1049211231

buffer.obj: ../buffer.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"/home/greg/ti/ti-cgt-c6000_8.2.2/bin/cl6x" --include_path="/home/greg/workspace_v7/ti-rtos-uart-test" --include_path="/home/greg/ti/ti-cgt-c6000_8.2.2/include" --include_path="/home/greg/ti/mathlib_c66x_3_1_2_1/packages" --include_path="/home/greg/ti/dsplib_c66x_3_4_0_0/packages" --define=USE_SEMAPHORES -g --c99 --relaxed_ansi --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="buffer.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

fft_task.obj: ../fft_task.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"/home/greg/ti/ti-cgt-c6000_8.2.2/bin/cl6x" --include_path="/home/greg/workspace_v7/ti-rtos-uart-test" --include_path="/home/greg/ti/ti-cgt-c6000_8.2.2/include" --include_path="/home/greg/ti/mathlib_c66x_3_1_2_1/packages" --include_path="/home/greg/ti/dsplib_c66x_3_4_0_0/packages" --define=USE_SEMAPHORES -g --c99 --relaxed_ansi --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="fft_task.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

float_buffer.obj: ../float_buffer.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"/home/greg/ti/ti-cgt-c6000_8.2.2/bin/cl6x" --include_path="/home/greg/workspace_v7/ti-rtos-uart-test" --include_path="/home/greg/ti/ti-cgt-c6000_8.2.2/include" --include_path="/home/greg/ti/mathlib_c66x_3_1_2_1/packages" --include_path="/home/greg/ti/dsplib_c66x_3_4_0_0/packages" --define=USE_SEMAPHORES -g --c99 --relaxed_ansi --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="float_buffer.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"/home/greg/ti/ti-cgt-c6000_8.2.2/bin/cl6x" --include_path="/home/greg/workspace_v7/ti-rtos-uart-test" --include_path="/home/greg/ti/ti-cgt-c6000_8.2.2/include" --include_path="/home/greg/ti/mathlib_c66x_3_1_2_1/packages" --include_path="/home/greg/ti/dsplib_c66x_3_4_0_0/packages" --define=USE_SEMAPHORES -g --c99 --relaxed_ansi --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="main.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

uart_synchronizer.obj: ../uart_synchronizer.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"/home/greg/ti/ti-cgt-c6000_8.2.2/bin/cl6x" --include_path="/home/greg/workspace_v7/ti-rtos-uart-test" --include_path="/home/greg/ti/ti-cgt-c6000_8.2.2/include" --include_path="/home/greg/ti/mathlib_c66x_3_1_2_1/packages" --include_path="/home/greg/ti/dsplib_c66x_3_4_0_0/packages" --define=USE_SEMAPHORES -g --c99 --relaxed_ansi --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="uart_synchronizer.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

uart_task.obj: ../uart_task.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"/home/greg/ti/ti-cgt-c6000_8.2.2/bin/cl6x" --include_path="/home/greg/workspace_v7/ti-rtos-uart-test" --include_path="/home/greg/ti/ti-cgt-c6000_8.2.2/include" --include_path="/home/greg/ti/mathlib_c66x_3_1_2_1/packages" --include_path="/home/greg/ti/dsplib_c66x_3_4_0_0/packages" --define=USE_SEMAPHORES -g --c99 --relaxed_ansi --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="uart_task.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

utils.obj: ../utils.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"/home/greg/ti/ti-cgt-c6000_8.2.2/bin/cl6x" --include_path="/home/greg/workspace_v7/ti-rtos-uart-test" --include_path="/home/greg/ti/ti-cgt-c6000_8.2.2/include" --include_path="/home/greg/ti/mathlib_c66x_3_1_2_1/packages" --include_path="/home/greg/ti/dsplib_c66x_3_4_0_0/packages" --define=USE_SEMAPHORES -g --c99 --relaxed_ansi --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="utils.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


