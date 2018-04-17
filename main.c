#include "buffer.h"
#include "fft_task.h"
#include "float_buffer.h"
#include "uart_synchronizer.h"
#include "uart_task.h"
#include "utils.h"

#include <assert.h>
#include <complex.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <ti/drv/uart/UART.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>
#include <xdc/std.h>

UartSynchronizer *sync;
Event_Handle read_event;
Buffer *read_buffer;

// Event IDs:
// 0: write UART write buffer
// 1: append UART read buffer onto FFT buffer
// 2: write coefficients from FFT buffer onto UART write buffer

Void test_print_task(const UArg sync_arg,
                     const UArg __attribute__((unused)) unused) {
    UartSynchronizer *const sync = (UartSynchronizer*) sync_arg;

    static const char *const string = "Hello, world!\n";
    const size_t length = strlen(string);

    us_write(sync, string, length);
}

static void initialize(void) {
    Error_Block error = make_error_block();

    read_buffer = bf_new(&error);
    Buffer *const write_buffer = bf_new(&error);
    FloatBuffer *const fft_buffer = fb_new(&error);
    float complex *const cmag_buffer =
        Memory_alloc(NULL, FLOAT_BUFFER_SIZE * sizeof(float complex),
                     8, &error);

    if (!cmag_buffer) {
        System_abort("initialize: unable to allocate float complex buffer\n");
    }

    read_event = event_new(&error);
    Event_Handle write_event = event_new(&error);
    Event_Handle fft_event = event_new(&error);

    sync = us_new(&error);

    UartWriteArgs write_args = { write_event, sync };
    Task_Handle write_task = task_new(&uart_write_task, &write_args,
                                      write_buffer, 4, &error);

    CalcFftEventPair fft_event_pair = { read_event, fft_event };
    CalcFftBufferPair fft_buffer_pair = { read_buffer, fft_buffer };
    Task_Handle fft_task = task_new(&calc_fft_task, &fft_event_pair,
                                    &fft_buffer_pair, 2, &error);

    PrintFftEventPair print_event_pair = { write_event, fft_event };
    PrintFftBufferTuple print_buffer_tuple = { write_buffer, fft_buffer,
                                               cmag_buffer };
    Task_Handle print_task = task_new(&print_fft_task, &print_event_pair,
                                      &print_buffer_tuple, 3, &error);

//    Task_Handle test_task = task_new(&test_print_task, sync, NULL, 1, &error);

    BIOS_start();

    Task_delete(&write_task);

    us_delete(sync);

    Event_delete(&write_event);
    Event_delete(&read_event);

    Memory_free(NULL, cmag_buffer, FLOAT_BUFFER_SIZE * sizeof(float complex));
    fb_delete(fft_buffer);
    bf_delete(write_buffer);
    bf_delete(read_buffer);
}

Int main() {
    initialize();

    BIOS_exit(0);
}
