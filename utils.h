#ifndef EECS373_GREGJM_SARUF_VPARASHA_UTILS_H
#define EECS373_GREGJM_SARUF_VPARASHA_UTILS_H

#include <ti/drv/uart/UART.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/Error.h>

Error_Block make_error_block(void);

Event_Handle event_new(Error_Block *error);

Task_Handle task_new(const Task_FuncPtr function, void *const first_arg,
                     void *const second_arg, Error_Block *error);

#endif
