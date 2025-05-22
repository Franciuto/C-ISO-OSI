/* C-ISO-OSI - Main (test) */
/* flusso di esempio */
#include "level7_application.h"
#include <stdio.h>
// variabili globali allocate staticamente nel segmento .bss e non in .data perchè già inizializzate (memset a 0)
    char p1[100] = {0};
    char m1[100] = {0};

int main(void) { // void perchè cosi la firma nello stack sarà più leggera
    printf("Almeno compila... (6 granted) \n"); // più di 5 e mezzo...
//    char* p1 = livello7_send("Ciao!\n");     // | @Franciuto non puoi definire un puntatore runtime!! spaiot!!
//    char* m1 = livello7_receive(p1);         // | In compile time gcc non sa con che cazzo inizializzare m1!!
    char *p = p1;
    char *m = m1;
    p = livello7_send("Ciao!\n"); // incapsulamento ↓
    m = livello7_receive(p1); // decapsulamento ↑

    printf("Ricevuto: %s", m1);
    return 0;
}
