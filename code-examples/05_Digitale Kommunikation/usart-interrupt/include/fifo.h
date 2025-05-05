#include <stm32f0xx.h>

#define FIFO_SIZE 64
#define FIFO_FULL_ERROR 1
#define FIFO_EMPTY_ERROR 2


typedef struct {
    uint8_t buffer[FIFO_SIZE];
    uint16_t head;
    uint16_t tail;
} Fifo_t;


void fifo_init(Fifo_t* fifo);
uint8_t fifo_put(Fifo_t* fifo, uint8_t data);
uint8_t fifo_get(Fifo_t* fifo, uint8_t* data);
