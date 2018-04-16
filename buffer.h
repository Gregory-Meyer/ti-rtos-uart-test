#ifndef EECS373_GREGJM_SARUF_VPARASHA_BUFFER_H
#define EECS373_GREGJM_SARUF_VPARASHA_BUFFER_H

#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Semaphore.h>
#include <xdc/runtime/Error.h>

#define BUFFER_SIZE 8192
#define BUFFER_ALIGN 16

typedef struct Buffer {
    uint8_t __attribute__((aligned(BUFFER_ALIGN))) data[BUFFER_SIZE];
    size_t size;
    Semaphore_Handle semaphore;
} Buffer;

Buffer* bf_new(Error_Block *error);

void bf_delete(Buffer *self);

size_t bf_append(Buffer *self, const void *buffer, size_t length);

size_t bf_size(const Buffer *self);

size_t bf_move(Buffer *self, void *buffer, size_t length);

#endif
