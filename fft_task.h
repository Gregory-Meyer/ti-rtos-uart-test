#ifndef EECS373_GREGJM_SARUF_VPARASHA_FFT_TASKS_H
#define EECS373_GREGJM_SARUF_VPARASHA_FFT_TASKS_H

#include "buffer.h"
#include "float_buffer.h"

#include <complex.h>

#include <ti/sysbios/knl/Event.h>

typedef struct CalcFftEventPair {
    Event_Handle event;
    Event_Handle fft_event;
} CalcFftEventPair;

typedef struct CalcFftBufferPair {
    Buffer *buffer;
    FloatBuffer *fft_buffer;
} CalcFftBufferPair;

typedef struct PrintFftEventPair {
    Event_Handle write_event;
    Event_Handle fft_event;
} PrintFftEventPair;

typedef struct PrintFftBufferTuple {
    Buffer *const write_buffer;
    FloatBuffer *const fft_buffer;
    float complex *const cmag_buffer;
} PrintFftBufferTuple;

Void calc_fft_task(UArg events_arg, UArg buffers_arg);

Void print_fft_task(UArg events_arg, UArg buffers_arg);

#endif
