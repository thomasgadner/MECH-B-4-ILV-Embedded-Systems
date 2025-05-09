#include <stm32f0xx.h>

#define FIFO_SIZE 64
#define FIFO_ERROR -1



typedef struct {
    uint8_t buffer[FIFO_SIZE];
    uint16_t head;
    uint16_t tail;
} Fifo_t;


void fifo_init(Fifo_t* fifo);
int fifo_put(Fifo_t* fifo, uint8_t data);
int fifo_get(Fifo_t* fifo, uint8_t* data);
