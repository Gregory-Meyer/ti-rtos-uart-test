#include "uart_task.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>

static void uart_read(const Event_Handle event, UartSynchronizer *const sync,
                      Buffer *const buffer) {
    uint8_t local_buffer[512];

    while (true) {
        const size_t read = us_read(sync, local_buffer, 512);

        if (read > 0) {
            bf_append(buffer, local_buffer, read);
        }

        if (bf_size(buffer) >= 256) {
            Event_post(event, Event_Id_01);
        }
    }
}

static void write_buffer(UartSynchronizer *const sync,
                         const void *const buffer, const size_t length) {
    size_t to_write = length;

    while (to_write > 0) {
        const void *first = (const uint8_t*) buffer + length - to_write;
        const size_t wrote = us_write(sync, first, to_write);

        to_write -= wrote;
    }
}

static void uart_write(const Event_Handle event, UartSynchronizer *const sync,
                       Buffer *const buffer) {
    uint8_t local_buffer[512];

    while (true) {
        Event_pend(event, Event_Id_NONE, Event_Id_00, BIOS_WAIT_FOREVER);

        while (bf_size(buffer) > 0) {
            const size_t moved = bf_move(buffer, local_buffer, 512);

            write_buffer(sync, local_buffer, moved);
        }
    }
}

Void uart_read_task(const UArg args_arg, const UArg buffer_arg) {
    UartReadArgs *const args = (UartReadArgs*) args_arg;
    const Event_Handle event = args->event;
    UartSynchronizer *const sync = args->sync;
    Buffer *const buffer = (Buffer*) buffer_arg;

    uart_read(event, sync, buffer);
}

Void uart_write_task(const UArg args_arg, const UArg buffer_arg) {
    UartWriteArgs *const args = (UartWriteArgs*) args_arg;
    const Event_Handle event = args->event;
    UartSynchronizer *const sync = args->sync;

    Buffer *const buffer = (Buffer*) buffer_arg;

    uart_write(event, sync, buffer);
}
