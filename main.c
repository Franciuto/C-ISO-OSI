/* C-ISO-OSI - Main (test) */
/* flusso di esempio */
#include "level7_application.h"
#include "level1_fisic.h"
#include "level2_datalink.h"
#include "level3_network.h"
#include "level4_transport.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) { // void perchè cosi la firma nello stack sarà più leggera
    printf("\n\t Almeno compila... (6 granted) \n\n"); // più di 5 e mezzo...
    
    // Initialize all layers
    printf("[Main] Initializing ISO/OSI stack layers...\n");
    
    // Initialize the physical layer FIFO queue
    livello1_init();
    
    // Initialize the transport layer reassembly buffers
    livello4_init();
    
    printf("[Main] All layers initialized successfully\n");
    
    // Send phase - encapsulation from L7 down to L1
    printf("\n\t--------- ISO/OSI Send stack -------\n\n");
    char* p1 = livello7_send("I use Arch BTW");
    
    if (p1 == NULL) {
        fprintf(stderr, "[Main] ERROR: Send failed\n");
        return 1;
    }
    
    free(p1); // Free the result from send phase
    
    printf("\n\t--------- ISO/OSI Receive stack -------\n\n");
    
    // Process receive path until we have all fragments
    char* received = NULL;
    char* final_result = NULL;
    
    // Process each fragment in order from Layer 3
    while ((received = livello3_receive(NULL)) != NULL) {
        printf("[Main] Processing fragment: %.30s...\n", received);
        
        // Pass each fragment to Layer 7 for processing
        char* result = livello7_receive(received);
        
        // If we got a result, it means the message is complete
        if (result != NULL) {
            printf("[Main] Received complete message\n");
            final_result = result;
        }
        
        // Free the current fragment
        free(received);
    }
    
    
    // Check if we got back the expected decoded message
    if (strcmp(final_result, "I use Arch BTW") == 0) {
        printf("✅ Received message \"%s\" is correct", final_result);
    } else {
        printf("⚠️ Message transmission issue detected!\n");
        printf("║ ⚠️ Expected: \"I use Arch BTW\" - Received: \"%s\"\n", final_result);
    }
    return 0;
}
