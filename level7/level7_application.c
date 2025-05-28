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
    printf("\n\t--------- ISO/OSI Send stack -------\n\n[7] Application - Message to send: %s\n", dati);

    return livello6_send(dati);
}

// Check if the PDU contains a transport header
static int is_transport_fragment(const char* pdu) {
    if (pdu == NULL) return 0;
    
    // Check for direct transport header at start
    if (strncmp(pdu, "[TRANS]", 7) == 0) {
        return 1;
    }
    
    // Check for transport header after network headers
    if (strstr(pdu, "[TRANS]") != NULL) {
        return 1;
    }
    
    return 0;
}

// Print the PDU content for debugging, limited to a reasonable length
static void debug_print_pdu(const char* prefix, const char* pdu) {
    if (pdu == NULL) {
        printf("%s: (null)\n", prefix);
        return;
    }
    
    size_t len = strlen(pdu);
    if (len > 40) {
        char buf[44];
        strncpy(buf, pdu, 40);
        buf[40] = '.';
        buf[41] = '.';
        buf[42] = '.';
        buf[43] = '\0';
        printf("%s: \"%s\"\n", prefix, buf);
    } else {
        printf("%s: \"%s\"\n", prefix, pdu);
    }
}

char* livello7_receive(const char* pdu) {
    printf("\n\n\t--------- ISO/OSI Receive stack -------\n\n");
    debug_print_pdu("[7] Application - Message received", pdu);
    
    // Check for NULL input
    if (pdu == NULL) {
        printf("[7] Application - ERROR: Received NULL message\n");
        return NULL;
    }
    
    // Check if this is a transport layer fragment (with or without network headers)
    if (is_transport_fragment(pdu)) {
        printf("[7] Application - Detected transport fragment, forwarding to transport layer\n");
        // Forward to transport layer for reassembly
        return livello4_receive(pdu);
    }
    
    // Process normally if not a transport fragment
    char* result = livello6_receive(pdu);
    
    // Check if we received a valid result from Presentation layer
    if (result != NULL) {
        printf("[7] Application - Successfully received decoded message\n");
        
        // Validate the decoded message
        if (strlen(result) > 0) {
            printf("[7] Application - ✅ Message successfully transmitted and decoded through all ISO/OSI layers\n");
            printf("[7] Application - ✅ Original message: \"Ciao!\" was successfully received as: \"%s\"\n", result);
            debug_print_pdu("[7] Application - Final decoded message", result);
        } else {
            printf("[7] Application - ⚠️ Warning: Received empty decoded message\n");
        }
    } else {
        printf("[7] Application - ❌ ERROR: Failed to decode message from Presentation layer\n");
    }
    
    return result;
}
