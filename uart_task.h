#ifndef EECS373_GREGJM_SARUF_VPARASHA_UART_TASK_H
#define EECS373_GREGJM_SARUF_VPARASHA_UART_TASK_H

#include "buffer.h"
#include "uart_synchronizer.h"

#include <ti/sysbios/knl/Event.h>

typedef struct UartReadArgs {
    Event_Handle event;
    UartSynchronizer *sync;
} UartReadArgs;

typedef struct UartWriteArgs {
    Event_Handle event;
    UartSynchronizer *sync;
} UartWriteArgs;

Void uart_read_task(void);

Void uart_write_task(UArg args_arg, UArg buffer_arg);

#endif
