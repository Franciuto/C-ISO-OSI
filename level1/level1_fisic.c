/* C-ISO-OSI - Physical layer - Franciuto */

// LIBRARY HEADERS
#include "constants.h"  // Library constants
#include "level1_fisic.h"

// STANDARD HEADERS
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define FIFO_SIZE 16

// FIFO initialization
static char* fifo[FIFO_SIZE];
static int fifo_head = 0;
static int fifo_tail = 0;
static int fifo_count = 0;

// Current pointing pos
static int current_recv_pos = 0;

void livello1_send(const char* bitstream) {
   // FIll the fifo with bitstream
    fifo[fifo_tail] = strdup(bitstream);
    
    printf("[L1] Physical - Stored in FIFO: %s\n", bitstream);
    
    // Update pointer
    fifo_tail = (fifo_FIFOe() {
    // Pop from fifo
    char* data = fifo[fifo_head];
    
    // Update FIFO pointers
    fifo_head = (fifo_head + 1) % FIFO_SIZE;
    fifo_count--;
    
    printf("[L1] Physical - POP from FIFO: %s\n", data);
    
    return data;
}

// Initialization for level1
void livello1_init() {
    // Clear the fifo
    for (int i = 0; i < FIFO_SIZE; i++) {
        if (fifo[i] != NULL) {
            free(fifo[i]);
            fifo[i] = NULL;
        }
    }
    
    // Reset fifo pointer
    fifo_head = 0;
    fifo_tail = 0;
    fifo_count = 0;
    current_recv_pos = 0;
}
