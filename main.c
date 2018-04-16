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
#include <math.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/drv/uart/UART.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>


#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#include <ti/mathlib/mathlib.h>
#include <ti/dsplib/dsplib.h>

// Event IDs:
// 0: write UART write buffer
// 1: append UART read buffer onto FFT buffer
// 2: write coefficients from FFT buffer onto UART write buffer

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

static void fft(const Event_Handle event, const Event_Handle fft_event,
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
    Event_Handle write_event;
    Event_Handle fft_event;
} PrintFftEventPair;

typedef struct PrintFftBufferTuple {
    Buffer *const write_buffer;
    FloatBuffer *const fft_buffer;
    float complex *const cmag_buffer;
} PrintFftBufferTuple;

static void
print_fft(const Event_Handle write_event, const Event_Handle fft_event,
          Buffer *const write_buffer, FloatBuffer *const fft_buffer,
          float complex *const cmag_buffer) {
    float *const mag_buffer = (float*) cmag_buffer;

    // frequencies (Hz):
    // 25, 50, 100, 200, 400, 800, 1600, 2400, 3200, 4800, 6400
    // indices (40kHz, 1024 samples):
    // [0, 2), [1, 3), [2, 4), [4, 7), [9, 13), [18, 24),
    // [36, 47), [55, 69), [73, 92), [110, 137), [147, 182)
    uint8_t magnitudes[11];

    // these are offset to be double word aligned
    static const size_t INDEX[11][2] =
        { { 0, 2 }, { 0, 3 }, { 2, 2 }, { 4, 3 }, { 8, 5 },
          { 18, 6 }, { 36, 11 }, { 54, 15 }, { 72, 20 }, { 110, 27 },
          { 146, 36 } };

    while (true) {
        Event_pend(fft_event, Event_Id_NONE, Event_Id_02, BIOS_WAIT_FOREVER);

        fb_fft_drain(fft_buffer, cmag_buffer);

        for (size_t i = 0; i < FLOAT_BUFFER_SIZE; ++i) {
            const float re = mag_buffer[2 * i];
            const float im = mag_buffer[2 * i + 1];

            mag_buffer[i] = sqrtsp(sqrtsp(re * re + im * im)
                                   * 2.0f / FLOAT_BUFFER_SIZE);
        }

        for (size_t i = 0; i < 11; ++i) {
            float mag = DSPF_sp_vecsum_sq(mag_buffer + INDEX[i][0],
                                          (int) INDEX[i][1]);

            mag /= (float) FLOAT_BUFFER_SIZE;

            if (mag > 1.0f) {
                mag = 1.0f;
            }

            mag = round(mag * 10.0f);

            magnitudes[i] = (uint8_t) mag;
        }

        bf_append(write_buffer, magnitudes, 11);

        Event_post(write_event, Event_Id_00);
    }
}

Void print_fft_task(const UArg events_arg, const UArg buffers_arg) {
    PrintFftEventPair *const events = (PrintFftEventPair*) events_arg;
    PrintFftBufferTuple *const buffers = (PrintFftBufferTuple*) buffers_arg;

    const Event_Handle write_event = events->write_event;
    const Event_Handle fft_event = events->fft_event;

    Buffer *const write_buffer = buffers->write_buffer;
    FloatBuffer *const fft_buffer = buffers->fft_buffer;
    float complex *const cmag_buffer = buffers->cmag_buffer;

    print_fft(write_event, fft_event, write_buffer, fft_buffer, cmag_buffer);
}

static void initialize(void) {
    Error_Block error = make_error_block();

    Buffer *const read_buffer = bf_new(&error);
    Buffer *const write_buffer = bf_new(&error);
    FloatBuffer *const fft_buffer = fb_new(&error);
    float complex *const cmag_buffer =
        Memory_alloc(NULL, 1024 * sizeof(float complex), 8, &error);

    if (!cmag_buffer) {
        System_abort("initialize: unable to allocate float complex buffer\n");
    }

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

    PrintFftEventPair print_event_pair = { write_event, fft_event };
    PrintFftBufferTuple print_buffer_tuple = { write_buffer, fft_buffer,
                                               cmag_buffer };
    Task_Handle print_task = task_new(&print_fft_task, &print_event_pair,
                                      &print_buffer_tuple, &error);

    BIOS_start();

    Task_delete(&write_task);
    Task_delete(&read_task);

    us_delete(sync);

    Event_delete(&write_event);
    Event_delete(&read_event);

    Memory_free(NULL, cmag_buffer, 1024 * sizeof(float complex));
    fb_delete(fft_buffer);
    bf_delete(write_buffer);
    bf_delete(read_buffer);
}

Int main() {
    initialize();

    BIOS_exit(0);
}
