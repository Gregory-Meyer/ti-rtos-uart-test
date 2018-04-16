#include "buffer.h"
#include "uart_synchronizer.h"
#include "utils.h"
#include "float_buffer.h"

#include <assert.h>
#include <complex.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/drv/uart/UART.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>

static void uart_read(const Event_Handle event, UartSynchronizer *const sync,
                      Buffer *const buffer) {
    while (true) {
        uint8_t local_buffer[1];

        const size_t read = us_read(sync, local_buffer, 1);

        if (read > 0) {
            bf_append(buffer, local_buffer, 1);

            Event_post(event, Event_Id_01);
        }
    }
}

typedef struct UartReadArgs {
    Event_Handle event;
    UartSynchronizer *sync;
} UartReadArgs;

Void uart_read_task(const UArg args_arg, const UArg buffer_arg) {
    UartReadArgs *const args = (UartReadArgs*) args_arg;
    const Event_Handle event = args->event;
    UartSynchronizer *const sync = args->sync;
    Buffer *const buffer = (Buffer*) buffer_arg;

    uart_read(event, sync, buffer);
}

typedef struct UartWriteArgs {
    Event_Handle event;
    UartSynchronizer *sync;
} UartWriteArgs;

static void uart_write(const Event_Handle event, UartSynchronizer *const sync,
                       Buffer *const buffer) {
    while (true) {
        Event_pend(event, Event_Id_NONE, Event_Id_00, BIOS_WAIT_FOREVER);

        uint8_t local_buffer[512];

        while (bf_size(buffer) > 0) {
            const size_t moved = bf_move(buffer, local_buffer, 512);
            size_t to_write = moved;

            while (to_write > 0) {
                const void *buf_start = local_buffer + moved - to_write;
                const size_t wrote = us_write(sync, buf_start, to_write);

                to_write -= wrote;
            }
        }
    }
}

Void uart_write_task(const UArg args_arg, const UArg buffer_arg) {
    UartWriteArgs *const args = (UartWriteArgs*) args_arg;
    const Event_Handle event = args->event;
    UartSynchronizer *const sync = args->sync;

    Buffer *const buffer = (Buffer*) buffer_arg;

    uart_write(event, sync, buffer);
}

Void uart_print_task(const UArg event_arg, const UArg buffer_arg) {
    static const char *const string = "Hello, world!";
    const size_t length = strlen(string);

    const Event_Handle event = (Event_Handle) event_arg;
    Buffer *const buffer = (Buffer*) buffer_arg;

    bf_append(buffer, string, length);
    Event_post(event, Event_Id_00);
}

void fft(const Event_Handle event, const Event_Handle fft_event,
         Buffer *const input_buffer, FloatBuffer *const fft_buffer) {
    while (true) {
        Event_pend(event, Event_Id_NONE, Event_Id_01, BIOS_WAIT_FOREVER);

        fb_append(fft_buffer, input_buffer);

        if (fb_size(fft_buffer) == 1024) {
            Event_post(fft_event, Event_Id_02);
        }
    }
}

typedef struct FftEventPair {
    Event_Handle event;
    Event_Handle fft_event;
} FftEventPair;

typedef struct FftBufferPair {
    Buffer *buffer;
    FloatBuffer *fft_buffer;
} FftBufferPair;

Void fft_task(const UArg events_arg, const UArg buffers_arg) {
    FftEventPair *const events = (FftEventPair*) events_arg;
    FftBufferPair *const buffers = (FftBufferPair*) buffers_arg;

    const Event_Handle event = events->event;
    const Event_Handle fft_event = events->fft_event;

    Buffer *const buffer = buffers->buffer;
    FloatBuffer *const fft_buffer = buffers->fft_buffer;

    fft(event, fft_event, buffer, fft_buffer);
}

typedef struct PrintFftEventPair {
    Event_Handle fft_event;
    Event_Handle write_event;
} PrintFftEventPair;

typedef struct PrintFftBufferPair {
    Buffer *const write_buffer;
    FloatBuffer *const fft_buffer;
} PrintFftBufferPair;

Void print_fft_task(const UArg args_arg, const UArg fft_buffer) {

}

static void initialize(void) {
    Error_Block error = make_error_block();

    Buffer *const read_buffer = bf_new(&error);
    Buffer *const write_buffer = bf_new(&error);
    FloatBuffer *const fft_buffer = fb_new(&error);

    Event_Handle read_event = event_new(&error);
    Event_Handle write_event = event_new(&error);
    Event_Handle fft_event = event_new(&error);

    UartSynchronizer *const sync = us_new(&error);

    UartReadArgs read_args = { read_event, sync };
    Task_Handle read_task = task_new(&uart_read_task, &read_args,
                                     read_buffer, &error);

    UartWriteArgs write_args = { write_event, sync };
    Task_Handle write_task = task_new(&uart_write_task, &write_args,
                                      write_buffer, &error);

    Task_Handle print_task = task_new(&print_fft_task, write_event,
                                      write_buffer, &error);

    BIOS_start();

    Task_delete(&write_task);
    Task_delete(&read_task);

    us_delete(sync);

    Event_delete(&write_event);
    Event_delete(&read_event);

    bf_delete(write_buffer);
    bf_delete(read_buffer);
}

Int main() {
    initialize();

    BIOS_exit(0);
}
