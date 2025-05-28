/* C-ISO-OSI - Data Link Layer - Functions - Francesco Fontanesi

    - Appends a control code (checksum) at the end of the payload for error detection.
    - On receiving a frame, verifies the checksum (CRC).
    - If the checksum is correct, forwards the payload to the upper layer.
    - If errors are detected, discards the frame without forwarding.

    Ensures reliable data transfer across the physical link by detecting and handling transmission errors.
*/

/* LIBRARY HEADERS */
#include "constants.h"  // Library constants
#include "level1_fisic.h"
#include "level2_datalink.h"

/* STANDARD HEADERS */
unsigned char calculate_checksum(const char* data) {
    unsigned int sum = 0;
    for (int i = 0; data[i]; ++i) {
        sum += (unsigned char)data[i];
    }
    return sum % 256;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Simple frame validation function
static int validate_frame(const char* frame) {
    // For this simulation, we'll just check if the frame is not NULL and not empty
    if (frame == NULL || frame[0] == '\0') {
        return 0; // Invalid frame
    }
    
    // Additional validation could be added here (e.g., checksum verification)
    return 1; // Valid frame
}

// Function to receive a frame from Layer 1
char* livello2_receive(const char* pdu) {
    // The pdu parameter is ignored as it's coming from Layer 3
    // Instead, we get the data from Layer 1
    char* frame = livello1_receive();
    
    // Check if frame is NULL (no more frames in FIFO)
    if (frame == NULL) {
        printf("[L2] Datalink - No more frames available from Physical layer\n");
        return NULL;
    }
    
    // Validate the frame
    char received_checksum[3];
    char received_data[PDU_SIZE];
    unsigned char calc_checksum;
    sscanf(frame, "%[^[][CHK=%2s]", received_data, received_checksum);
    calc_checksum = calculate_checksum(received_data);
    
    char hex_checksum[3];
    snprintf(hex_checksum, 3, "%02X", calc_checksum);

    if (strcmp(hex_checksum, received_checksum) != 0) {
        printf("[L2] Datalink ERROR - Checksum error! Frame discarded. Received checksum: %s, Calculated checksum: %s\n", received_checksum, hex_checksum);
        free(frame);
        return NULL;
    }
    
    printf("[L2] Datalink - Received valid frame: %s\n", frame);
    
    // Return the frame to Layer 3
    // Note: The memory will be freed by the upper layer
    return frame;
}

// Function to send a frame to Layer 1
char* livello2_send(const char* dati) {
    if (dati == NULL) {
        printf("[L2] Datalink ERROR - Cannot send NULL data\n");
        return NULL;
    }
    
    printf("[L2] Datalink - Sending frame: %s\n", dati);
    
    // In a real implementation, we would add a checksum here
    // For this simulation, we'll just pass the data to Layer 1
    unsigned char checksum = calculate_checksum(dati);
    char frame_with_checksum[PDU_SIZE];
    snprintf(frame_with_checksum, sizeof(frame_with_checksum), "%s[CHK=%02X]", dati, checksum);
    livello1_send(frame_with_checksum);
    
    return strdup(dati); // Return a copy of the sent data
}
