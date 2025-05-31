/* C-ISO-OSI - Application layer - Functions

    - Receives input from the user and displays output, typically in the form of text, images, commands.
    - Allows the application to use network protocols (such as HTTP, FTP, SMTP, DNS).
    - Provides network services directly to end-user applications.
    - Interfaces with the presentation layer to send and receive data.

    Takes data from an application (e.g., a chat message), passes it to the presentation layer for further processing, and handles received data from the presentation layer for use by the application.
*/

/* LIBRARY HEADERS */
#include "constants.h"  // Library constants
#include "level7_application.h"
#include "level6_presentation.h"
#include "level4_transport.h"

/* STANDARD HEADERS */
#include <stdio.h>
#include <string.h>

char* livello7_send(const char* dati) {
    printf("\n───────────────────────────────────────────────── ISO/OSI Send stack ────────────────────────────────────────\n\n[7] Application - Message to send: %s\n", dati);
    livello6_send(dati, "ROT13");
    return dati;
}

char* livello7_receive() {
    printf("\n\n───────────────────────────────────────────── ISO/OSI Receive stack ────────────────────────────────────\n\n");
            
    char* result = livello6_receive();
    
    if (result != NULL) {
        if (strlen(result) > 0) {
            printf("[7] Application - ✅ Message successfully transmitted and decoded through all ISO/OSI layers\n\n\n");
        }
    }    
    return result;
}
