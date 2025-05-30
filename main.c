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

    // Initialize the physical layer FIFO queue
    livello1_init();

    char* p1 = livello7_send("I use Arch BTW");
    printf("\n\n\t\aPDU TO PASS TO THE RECEIVE STACK:  %s\n\n", p1);
    char *m1 = livello7_receive(p1);

    if (*p1) free(p1); // Free the result from send phase

    // Check if we got back the expected decoded message
    if (strcmp(m1, "I use Arch BTW") == 0) {
        printf("✅ Received message \"%s\" is correct", m1);
    } else {
        printf("⚠️ Message transmission issue detected!\n");
        printf("║ ⚠️ Expected: \"I use Arch BTW\" - Received: \"%s\"\n", m1);
    }
    return 0;
}
