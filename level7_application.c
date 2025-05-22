/* C-ISO-OSI - Application layer - Functions */
#include "level7_application.h"
#include "level6_presentation.h"
#include <stdio.h>
/*
COMPITO DEL LIVELLO 7:
- Riceve input dall'utente e mostra l’output, tipicamente in forma di testo, immagini, comandi.
- Permette all'applicazione di usare i protocolli di rete (come HTTP, FTP, SMTP, DNS).

Prende i dati da un'applicazione (es. un messaggio di chat) e li manda verso il livello 6 per l'incapsulamento.
*/

char* livello7_send(const char* dati) {
    printf("[L7] Applicazione - Messaggio da inviare: %s\n", dati);

    return livello6_send(dati);
}

char* livello7_receive(const char* pdu) {
    char* dati = livello6_receive(pdu);

    printf("[L7] Applicazione - Messaggio ricevuto: %s\n", dati);
    return dati;
}
