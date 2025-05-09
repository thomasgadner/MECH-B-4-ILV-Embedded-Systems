#include "fifo.h"


void fifo_init(Fifo_t* fifo) {
    fifo->head = 0;                              // Initialize head pointer to 0
    fifo->tail = 0;                              // Initialize tail pointer to 0
}

uint8_t fifo_is_empty(Fifo_t* fifo) {
    return (fifo->head == fifo->tail);          // FIFO is empty if head and tail are equal
}

uint8_t fifo_is_full(Fifo_t* fifo) {
    return ((fifo->head + 1) % FIFO_SIZE) == fifo->tail; // FIFO is full if incrementing head would equal tail
}

int fifo_put(Fifo_t* fifo, uint8_t data) {
    if (fifo_is_full(fifo)) {                   // Check if FIFO is full before inserting
        return -1;                               // Insertion failed (buffer full)
    }

    fifo->buffer[fifo->head] = data;            // Store data at current head position
    fifo->head = (fifo->head + 1) % FIFO_SIZE;  // Move head forward and wrap around if needed
    return 0;                                   // Insertion successful
}

int fifo_get(Fifo_t* fifo, uint8_t* data) {
    if (fifo_is_empty(fifo)) {                  // Check if FIFO is empty before reading
        return -1;                               // Read failed (buffer empty)
    }

    *data = fifo->buffer[fifo->tail];           // Retrieve data at current tail position
    fifo->tail = (fifo->tail + 1) % FIFO_SIZE;  // Move tail forward and wrap around if needed
    return 0;                          // Read successful Return 0
}