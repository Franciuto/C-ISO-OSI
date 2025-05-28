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
    char* p1 = livello7_send("Ciao!");
    
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
    
    // Check if we have a final result
    if (final_result != NULL) {
        printf("\n");
        printf("╔════════════════════════════════════════════════════════╗\n");
        printf("║                 ISO/OSI SIMULATION RESULT               ║\n");
        printf("╠════════════════════════════════════════════════════════╣\n");
        printf("║ Original message: \"Ciao!\"                               ║\n");
        printf("║                                                        ║\n");
        printf("║ Message sent through ISO/OSI stack:                    ║\n");
        printf("║   → Passed down through Application layer              ║\n");
        printf("║   → ROT13 encoded in Presentation layer: \"Pvnb!\"        ║\n");
        printf("║   → Session headers added                              ║\n");
        printf("║   → Split into 3 fragments by Transport layer          ║\n");
        printf("║   → Transmitted through Network, Datalink, Physical    ║\n");
        printf("║                                                        ║\n");
        printf("║ Message received:                                      ║\n");
        printf("║   → Fragments collected from Physical layer            ║\n");
        printf("║   → Reassembled in Transport layer                     ║\n");
        printf("║   → Session headers removed                            ║\n");
        printf("║   → ROT13 decoded in Presentation layer                ║\n");
        printf("║   → Final decoded message: \"%s\"                        ║\n", final_result);
        printf("║                                                        ║\n");
        
        // Check if we got back the expected decoded message
        if (strcmp(final_result, "Ciao!") == 0) {
            printf("║ ✅ Message successfully transmitted through all layers! ║\n");
            printf("║ ✅ ISO/OSI stack simulation completed successfully!    ║\n");
        } else {
            printf("║ ⚠️ Message transmission issue detected!                ║\n");
            printf("║ ⚠️ Expected: \"Ciao!\" - Received: \"%s\"                 ║\n", final_result);
        }
        
        printf("╚════════════════════════════════════════════════════════╝\n");
        printf("\n");
        
        free(final_result);
    } else {
        fprintf(stderr, "\n");
        fprintf(stderr, "╔════════════════════════════════════════════════════════╗\n");
        fprintf(stderr, "║               ISO/OSI SIMULATION ERROR                 ║\n");
        fprintf(stderr, "╠════════════════════════════════════════════════════════╣\n");
        fprintf(stderr, "║ Failed to receive and decode the complete message      ║\n");
        fprintf(stderr, "╚════════════════════════════════════════════════════════╝\n");
        fprintf(stderr, "\n");
    }
    
    printf("\n[Main] ISO/OSI simulation completed\n");
    return 0;
}
