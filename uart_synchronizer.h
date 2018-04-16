#ifndef EECS373_SARUF_GREGJM_VPARASHA_UART_SYNCHRONIZER_H
#define EECS373_SARUF_GREGJM_VPARASHA_UART_SYNCHRONIZER_H

#include <ti/sysbios/knl/Semaphore.h>
#include <ti/drv/uart/UART.h>
#include <xdc/runtime/Error.h>

typedef struct UartSynchronizer {
    UART_Handle uart;
    Semaphore_Handle semaphore;
} UartSynchronizer;

UartSynchronizer* us_new(Error_Block *error);

void us_delete(UartSynchronizer *self);

size_t us_read(UartSynchronizer *self, void *buffer, size_t length);

size_t us_write(UartSynchronizer *self, const void *buffer, size_t length);

#endif
