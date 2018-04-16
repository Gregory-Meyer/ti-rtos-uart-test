#include "float_buffer.h"

#include <string.h>

#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>

#include <ti/dsplib/dsplib.h>

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

    return self;
}

void fb_delete(FloatBuffer *const self) {
    Semaphore_delete(&self->semaphore);
    Memory_free(NULL, self, sizeof(FloatBuffer));
}

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

size_t fb_append(FloatBuffer *const self, const float complex *const buffer,
                 const size_t length) {
    Semaphore_pend(self->semaphore, BIOS_WAIT_FOREVER);

    const size_t num_to_append = MIN(length, FLOAT_BUFFER_SIZE - self->size);

    memcpy(self->data + self->size, buffer,
           num_to_append * sizeof(float complex));
    self->size += num_to_append;

    Semaphore_post(self->semaphore);

    return num_to_append;
}

size_t fb_size(const FloatBuffer *const self) {
    Semaphore_pend(self->semaphore, BIOS_WAIT_FOREVER);

    const size_t size = self->size;

    Semaphore_post(self->semaphore);

    return size;
}

size_t fb_fft_drain(FloatBuffer *const self, float complex *const output,
                    const size_t length) {

}

#undef MIN
