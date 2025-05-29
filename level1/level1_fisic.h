/* C-ISO-OSI - Physical layer - Functions */

// Include guard
#ifndef LEVEL1_FISIC_H
#define LEVEL1_FISIC_H

// Initialize the physical layer FIFO queue and buffers
void livello1_init();

// transmits the bit frame
void livello1_send(const char* bitstream);
// reads the bit frame from the transmission medium
char* livello1_receive();

#endif