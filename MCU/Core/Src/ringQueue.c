#include <ringQueue.h>

void rb_init(RingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
}

int rb_enqueue(RingBuffer *rb, uint8_t data) {
    uint16_t next = (rb->head + 1) & MASK;
    if (next == rb->tail) return -1; // Buffer full
    rb->buffer[rb->head] = data;
    rb->head = next;
    return 0;
}

int rb_dequeue(RingBuffer *rb, uint8_t *data) {
    if (rb->head == rb->tail) return -1; // Buffer empty
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) & MASK;
    return 0;
}
