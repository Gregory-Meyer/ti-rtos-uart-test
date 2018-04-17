#include "fft_task.h"

#include <stdbool.h>
#include <math.h>

#include <ti/sysbios/BIOS.h>

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#include <ti/mathlib/mathlib.h>
#include <ti/dsplib/dsplib.h>

static void
calc_fft(const Event_Handle event, const Event_Handle fft_event,
         Buffer *const input_buffer, FloatBuffer *const fft_buffer) {
    while (true) {
        Event_pend(event, Event_Id_NONE, Event_Id_01, BIOS_WAIT_FOREVER);

        fb_append(fft_buffer, input_buffer);

        if (fb_size(fft_buffer) >= 1024) {
            Event_post(fft_event, Event_Id_02);
        }
    }
}

static void calculate_magnitudes(float *const complex_mag) {
    for (size_t i = 0; i < FLOAT_BUFFER_SIZE; ++i) {
        const float re = complex_mag[2 * i];
        const float im = complex_mag[2 * i + 1];

        complex_mag[i] = sqrtsp(sqrtsp(re * re + im * im)
                                * 2.0f / FLOAT_BUFFER_SIZE);
    }
}

static void calculate_spectral_density(const float *const magnitude,
                                       uint8_t *const magnitudes) {
    // these are offset to be double word aligned
    static const size_t INDEX[11][2] =
        { { 0, 2 }, { 0, 3 }, { 2, 2 }, { 4, 3 }, { 8, 5 },
          { 18, 6 }, { 36, 11 }, { 54, 15 }, { 72, 20 }, { 110, 27 },
          { 146, 36 } };

    for (size_t i = 0; i < 11; ++i) {
        float mag = DSPF_sp_vecsum_sq(magnitude + INDEX[i][0],
                                      (int) INDEX[i][1]);

        mag /= (float) FLOAT_BUFFER_SIZE;

        if (mag > 1.0f) {
            mag = 1.0f;
        }

        mag = round(mag * 10.0f);

        magnitudes[i] = (uint8_t) mag;
    }
}

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

    while (true) {
        Event_pend(fft_event, Event_Id_NONE, Event_Id_02, BIOS_WAIT_FOREVER);

        fb_fft_drain(fft_buffer, cmag_buffer);

        calculate_magnitudes(mag_buffer);
        calculate_spectral_density(mag_buffer, magnitudes);

        bf_append(write_buffer, magnitudes, 11);

        Event_post(write_event, Event_Id_00);
    }
}

Void calc_fft_task(const UArg events_arg, const UArg buffers_arg) {
    CalcFftEventPair *const events = (CalcFftEventPair*) events_arg;
    CalcFftBufferPair *const buffers = (CalcFftBufferPair*) buffers_arg;

    const Event_Handle event = events->event;
    const Event_Handle fft_event = events->fft_event;

    Buffer *const buffer = buffers->buffer;
    FloatBuffer *const fft_buffer = buffers->fft_buffer;

    calc_fft(event, fft_event, buffer, fft_buffer);
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
