#ifndef EECS373_GREGJM_SARUF_VPARASHA_FLOAT_BUFFER_H
#define EECS373_GREGJM_SARUF_VPARASHA_FLOAT_BUFFER_H

#include <stddef.h>
#include <complex.h>

#include <ti/sysbios/knl/Semaphore.h>
#include <xdc/runtime/Error.h>

#define FLOAT_BUFFER_SIZE 1024

typedef struct FloatBuffer {
    float complex data[FLOAT_BUFFER_SIZE];
    size_t size;
    Semaphore_Handle semaphore;
} FloatBuffer;

FloatBuffer* fb_new(Error_Block *error);

void fb_delete(FloatBuffer *self);

size_t fb_append(FloatBuffer *self, const float complex *buffer,
                 size_t length);

size_t fb_size(const FloatBuffer *self);

size_t fb_fft_drain(FloatBuffer *self, float complex *output, size_t length);

#endif
