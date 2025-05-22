/* C-ISO-OSI - Main (test) */
/* flusso di esempio */
#include "level7_application.h"
#include <stdio.h>

//    char* p1 = livello7_send("Ciao!\n");     // | @Franciuto non puoi definire un puntatore globale a una funzione!! spaiot!!
//    char* m1 = livello7_receive(p1);         // | In compile time gcc non sa con che cazzo inizializzare m1 e p1!!

int main(void) { // void perchè cosi la firma nello stack sarà più leggera
    printf("\n\t Almeno compila... (6 granted) \n\n"); // più di 5 e mezzo...
    char* p1 = livello7_send("Ciao!");     // incapsulamento ↓
    char* m1 = livello7_receive(p1);         // decapsulamento ↑
    printf("Ricevuto: %s", m1);
    return 0;
}
