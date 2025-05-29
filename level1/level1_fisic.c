/* C-ISO-OSI - Physical layer - Functions */
#include "constants.h"  // Library constants
#include "level1_fisic.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Define a larger queue for storing fragments during simulation
#define FIFO_SIZE 16

// FIFO buffer for storing fragments
static char* fifo[FIFO_SIZE];
static int fifo_head = 0;
static int fifo_tail = 0;
static int fifo_count = 0;

// Current receive position tracker
static int current_recv_pos = 0;

void livello1_send(const char* bitstream) {
    // Check if FIFO is full
    if (fifo_count >= FIFO_SIZE) {
        printf("[L1] ERROR: Physical layer FIFO buffer overflow!\n");
        return;
    }
    
    // Store a copy of the bitstream in the FIFO
    fifo[fifo_tail] = strdup(bitstream);
    if (fifo[fifo_tail] == NULL) {
        printf("[L1] ERROR: Failed to allocate memory for FIFO entry!\n");
        return;
    }
    
    printf("[L1] Physical - Stored fragment in FIFO: %s\n", bitstream);
    
    // Update FIFO pointers
    fifo_tail = (fifo_tail + 1) % FIFO_SIZE;
    fifo_count++;
}

char* livello1_receive() {
    // Check if FIFO is empty
    if (fifo_count == 0) {
        printf("[L1] Physical - FIFO is empty, no more fragments to receive\n");
        return NULL;
    }
    
    // Get the next fragment from the FIFO
    char* data = fifo[fifo_head];
    
    // Update FIFO pointers
    fifo_head = (fifo_head + 1) % FIFO_SIZE;
    fifo_count--;
    
    printf("[L1] Physical - Retrieved fragment from FIFO: %s\n", data);
    
    return data;
}

// Initialize the physical layer
void livello1_init() {
    // Clear the FIFO
    for (int i = 0; i < FIFO_SIZE; i++) {
        if (fifo[i] != NULL) {
            free(fifo[i]);
            fifo[i] = NULL;
        }
    }
    
    // Reset FIFO pointers
    fifo_head = 0;
    fifo_tail = 0;
    fifo_count = 0;
    current_recv_pos = 0;
}
