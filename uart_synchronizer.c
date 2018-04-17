#include "uart_synchronizer.h"

#include "utils.h"

#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>

static UART_Params make_params(void) {
    UART_Params params;
    UART_Params_init(&params);

    params.readMode = UART_MODE_BLOCKING;
    params.writeMode = UART_MODE_BLOCKING;

    params.readTimeout = 10;
    params.writeTimeout = BIOS_WAIT_FOREVER;

    params.readCallback = NULL;
    params.writeCallback = NULL;

    params.readReturnMode = UART_RETURN_FULL;

    params.readDataMode = UART_DATA_BINARY;
    params.writeDataMode = UART_DATA_BINARY;

    params.readEcho = UART_ECHO_OFF;

    params.baudRate = 921600;

    params.dataLength = UART_LEN_8;
    params.stopBits = UART_STOP_ONE;
    params.parityType = UART_PAR_NONE;

    return params;
}

static UART_Handle uart_new(void) {
    UART_Params params = make_params();
    const UART_Handle uart = UART_open(0, &params);

    if (!uart) {
        System_abort("uart_new: unable to open UART\n");
    }

    return uart;
}

UartSynchronizer* us_new(Error_Block *const error) {
    UartSynchronizer *const self = Memory_alloc(NULL, sizeof(UartSynchronizer),
                                                __alignof__(UartSynchronizer),
                                                error);

    if (!self) {
        System_abort("us_new: unable to allocate UartSynchronizer\n");
    }

    self->uart = uart_new();
    self->semaphore = Semaphore_create(1, NULL, error);

    if (!self->semaphore) {
        System_abort("us_new: unable to create Semaphore\n");
    }

    return self;
}

void us_delete(UartSynchronizer *const self) {
    UART_close(self->uart);
    Semaphore_delete(&self->semaphore);

    Memory_free(NULL, self, sizeof(UartSynchronizer));
}

size_t us_read(UartSynchronizer *self, void *const buffer,
               const size_t length) {
    Semaphore_pend(self->semaphore, BIOS_WAIT_FOREVER);

    const size_t num_read = UART_read(self->uart, buffer, length);

    Semaphore_post(self->semaphore);


    return num_read;
}

size_t us_write(UartSynchronizer *self, const void *const buffer,
                const size_t length) {
    Semaphore_pend(self->semaphore, BIOS_WAIT_FOREVER);

    const size_t num_wrote = UART_write(self->uart, buffer, length);

    Semaphore_post(self->semaphore);


    return num_wrote;
}
