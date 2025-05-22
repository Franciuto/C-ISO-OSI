/* C-ISO-OSI - Physical layer - Functions */

// Include guard
#ifndef LEVEL1_FISIC_H
#define LEVEL1_FISIC_H

// transmits the bit frame
void physical_medium_send(const char* bitstream);
// reads the bit frame from the transmission medium
char* physical_medium_receive();

#endif