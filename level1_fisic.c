/* C-ISO-OSI - Physical layer - Functions */
#include "level1_fisic.h"
#include <string.h>
#define FIFO_SIZE 1024

char* fifo[FIFO_SIZE];
int fifo_head = 0;
int fifo_tail = 0;

// transmits the bit frame
void physical_medium_send(const char* bitstream) {
    fifo[fifo_tail] = strdup(bitstream);  // Copia stringa
    fifo_tail = (fifo_tail + 1) % FIFO_SIZE;
}
// reads the bit frame from the transmission medium

char* physical_medium_receive() {
    if (fifo_head == fifo_tail) return NULL;  // FIFO vuota
    char* data = fifo[fifo_head];
    fifo_head = (fifo_head + 1) % FIFO_SIZE;
    return data;
}
