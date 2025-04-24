#ifndef LCD_RINGQUEUE_H
#define LCD_RINGQUEUE_H

#include <stdint.h>

#define RB_QUEUE_SIZE  64
#define MASK (RB_QUEUE_SIZE - 1)

typedef struct {
    uint8_t buffer[RB_QUEUE_SIZE];
    uint16_t head;
    uint16_t tail;
} RingBuffer;

//Not thread-safe
void rb_init(RingBuffer *rb);

int rb_enqueue(RingBuffer *rb, uint8_t data);

int rb_dequeue(RingBuffer *rb, uint8_t *data);

#endif //LCD_RINGQUEUE_H
