#include "buffer.h"

#include <string.h>

#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>

Buffer* bf_new(Error_Block *const error) {
    Buffer *const self = Memory_alloc(NULL, sizeof(Buffer),
                                      __alignof__(Buffer), error);

    if (!self) {
        System_abort("buffer_new: unable to allocate Buffer\n");
    }

    self->size = 0;
    self->semaphore = Semaphore_create(1, NULL, error);

    if (!self->semaphore) {
        System_abort("buffer_new: unable to create Semaphore\n");
    }

    return self;
}

void bf_delete(Buffer *const self) {
    Semaphore_delete(&self->semaphore);
    Memory_free(NULL, self, sizeof(Buffer));
}

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

size_t bf_append(Buffer *const self, const void *const buffer,
               const size_t length) {
    Semaphore_pend(self->semaphore, BIOS_WAIT_FOREVER);

    const size_t num_to_append = MIN(length, BUFFER_SIZE - self->size);

    memcpy(self->data + self->size, buffer, num_to_append);
    self->size += num_to_append;

    Semaphore_post(self->semaphore);

    return num_to_append;
}

size_t bf_size(const Buffer *const self) {
    Semaphore_pend(self->semaphore, BIOS_WAIT_FOREVER);

    const size_t size = self->size;

    Semaphore_post(self->semaphore);

    return size;
}

size_t bf_move(Buffer *const self, void *const buffer,
               const size_t length) {
    Semaphore_pend(self->semaphore, BIOS_WAIT_FOREVER);

    const size_t num_to_move = MIN(length, self->size);
    const size_t remaining = self->size - num_to_move;

    memcpy(buffer, self->data, num_to_move);

    memmove(self->data, self->data + num_to_move, remaining);
    self->size = remaining;

    Semaphore_post(self->semaphore);

    return num_to_move;
}

#undef MIN
