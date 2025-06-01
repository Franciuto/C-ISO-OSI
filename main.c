/* C-ISO-OSI - Main (Edited by Giordano Fornari at 04:06 AM after only 2 cups of coffe) */
/* flusso di esempio */
#include "level7_application.h"
#include "level5_session.h"
#include "level1_fisic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // per srand e time


int main(void) { // void perchè cosi la firma nello stack sarà più leggera
    srand(time(NULL)); // inizializza il generatore di numeri casuali per l'id di sessione
    // memset a 0 del buffer circolare FIFO
    livello1_init();
    livello5_send("", "INIT"); // inizio sessione

    printf("\033[2J\033[H \nISO - OSI Stack Simulator\n\nv 1.0.0\n\nDeveloped by 🯆 Giordano Fornari, 🯉 Giorgia Folloni, 🯈 Francesco Fontanesi\nCi scusiamo per l'utilizzo di due lingue diverse. Purtroppo non tutti i developer conoscono l'italiano\n\nUlteriori informazioni e contatti disponibili qui: \t https://github.com/Franciuto/C-ISO-OSI\n\n\n");
//    printf("\n\t Almeno compila... (6 granted) \n\n"); // più di 5 e mezzo...
    char *input = (char *)malloc(100*1);
    
    while (1) {
        printf("\nMessaggio da inviare (CLOSE PER CHIUDERE): ");
        scanf("%s", input);
        if (strcmp(input, "CLOSE") == 0) break;
        // send stack start here
        char* p1 = livello7_send(input);

        // receive stack here    
        char* m1 = livello7_receive();

        
        // Check if we got back the expected decoded message
        if (strcmp(m1, input) == 0) {
            printf("✅ Messaggio ricevuto!: \"%s\"", m1);
        } else {
            printf("Sie è verificato un problema imprevisto. Contattare gli sviluppatori per informazioni.\n");
            printf("⚠️ \"%s\" != \"%s\"\n", input, m1);
    }
}
    free(input); // libera la memoria allocata per l'input
    printf("\n───────────────────────────────── Invio di chiusura sessione... ──────────────────────────────────\n\n");
    livello5_send("", "CLOSE");
    printf("\n───────────────────────────────── Ricezione di chiusura sessione... ──────────────────────────────────\n\n");
    livello5_receive(); // per ricevere la chiusura di sessione
    return 0;
}
/* Ringraziamento speciale a Fornari che ha speso una INTERA NOTTE per far funzionare al meglio questo progetto che fino al 29/05/2025 alle 19:03 non aveva una funzione che non desse Adress boundary errors */
