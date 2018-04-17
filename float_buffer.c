#include "float_buffer.h"

#include "utils.h"

#include <string.h>

#include <ti/sysbios/BIOS.h>
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

static void generate_twiddle_factors(FloatBuffer *const self) {
    float *const w = (float*) self->twiddle_factors;

    // from ti's fft_sp_ex
    int i, j, k;
    double x_t, y_t, theta1, theta2, theta3;
    static const double PI = 3.14159265358979324;

    for (j = 1, k = 0; j <= FLOAT_BUFFER_SIZE >> 2; j = j << 2)
    {
        for (i = 0; i < FLOAT_BUFFER_SIZE >> 2; i += j)
        {
            theta1 = 2 * PI * i / FLOAT_BUFFER_SIZE;
            x_t = cosdp(theta1);
            y_t = sindp(theta1);
            w[k] = (float) x_t;
            w[k + 1] = (float) y_t;

            theta2 = 4 * PI * i / FLOAT_BUFFER_SIZE;
            x_t = cosdp(theta2);
            y_t = sindp(theta2);
            w[k + 2] = (float) x_t;
            w[k + 3] = (float) y_t;

            theta3 = 6 * PI * i / FLOAT_BUFFER_SIZE;
            x_t = cosdp(theta3);
            y_t = sindp(theta3);
            w[k + 4] = (float) x_t;
            w[k + 5] = (float) y_t;
            k += 6;
        }
    }
}

static void set_bit_reversal_table(FloatBuffer *const self) {
    static unsigned char BIT_REVERSAL_TABLE[64] = {
        0x0, 0x20, 0x10, 0x30, 0x8, 0x28, 0x18, 0x38,
        0x4, 0x24, 0x14, 0x34, 0xc, 0x2c, 0x1c, 0x3c,
        0x2, 0x22, 0x12, 0x32, 0xa, 0x2a, 0x1a, 0x3a,
        0x6, 0x26, 0x16, 0x36, 0xe, 0x2e, 0x1e, 0x3e,
        0x1, 0x21, 0x11, 0x31, 0x9, 0x29, 0x19, 0x39,
        0x5, 0x25, 0x15, 0x35, 0xd, 0x2d, 0x1d, 0x3d,
        0x3, 0x23, 0x13, 0x33, 0xb, 0x2b, 0x1b, 0x3b,
        0x7, 0x27, 0x17, 0x37, 0xf, 0x2f, 0x1f, 0x3f
    };

    memcpy(self->bit_reversal_table, BIT_REVERSAL_TABLE, sizeof(BIT_REVERSAL_TABLE));
}

FloatBuffer* fb_new(Error_Block *const error) {
    FloatBuffer *const self = Memory_alloc(NULL, sizeof(FloatBuffer),
                                           __alignof__(FloatBuffer), error);

    if (!self) {
        System_abort("fb_new: unable to allocate FloatBuffer\n");
    }

    self->size = 0;
    self->semaphore = Semaphore_create(1, NULL, error);

    if (!self->semaphore) {
        System_abort("fb_new: unable to allocate Semaphore\n");
    }

    generate_twiddle_factors(self);
    set_bit_reversal_table(self);

    return self;
}

void fb_delete(FloatBuffer *const self) {
    Semaphore_delete(&self->semaphore);
    Memory_free(NULL, self, sizeof(FloatBuffer));
}

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

static float fi8_as_fl32(const uint8_t fixed) {
    const float as_fl = (float) fixed / (float) (1 << 8);
    const float offset = ((float) as_fl) - 0.5f;
    const float scaled = offset * 2.0f;

    return scaled;
}

size_t fb_append(FloatBuffer *const self, Buffer *const buffer) {
#ifdef USE_SEMAPHORES
    Semaphore_pend(self->semaphore, BIOS_WAIT_FOREVER);
    Semaphore_pend(buffer->semaphore, BIOS_WAIT_FOREVER);
#endif

    const size_t num_to_append =
        MIN(buffer->size, FLOAT_BUFFER_SIZE - self->size);

    for (size_t i = 0; i < num_to_append; ++i, ++self->size) {
        self->data[self->size] = fi8_as_fl32(buffer->data[i]) + 0.0 * I;
    }

    const size_t unappended = BUFFER_SIZE - num_to_append;
    memmove(buffer->data, buffer->data + num_to_append, unappended);
    buffer->size -= num_to_append;

#ifdef USE_SEMAPHORES
    Semaphore_post(self->semaphore);
    Semaphore_post(buffer->semaphore);
#endif

    return num_to_append;
}

size_t fb_size(const FloatBuffer *const self) {
#ifdef USE_SEMAPHORES
    Semaphore_pend(self->semaphore, BIOS_WAIT_FOREVER);
#endif

    const size_t size = self->size;

#ifdef USE_SEMAPHORES
    Semaphore_post(self->semaphore);
#endif

    return size;
}

void fb_fft_drain(FloatBuffer *const self, float complex *const output) {
#ifdef USE_SEMAPHORES
    Semaphore_pend(self->semaphore, BIOS_WAIT_FOREVER);
#endif

    float *const x = (float*) self->data;
    float *const w = (float*) self->twiddle_factors;
    float *const X = (float*) output;
    unsigned char *const brev = self->bit_reversal_table;

    DSPF_sp_fftSPxSP(FLOAT_BUFFER_SIZE, x, w, X,
                     brev, 4, 0, FLOAT_BUFFER_SIZE);

    self->size = 0;

#ifdef USE_SEMAPHORES
    Semaphore_post(self->semaphore);
#endif
}

#undef MIN
