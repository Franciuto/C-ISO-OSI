/* C-ISO-OSI - Main (test) */
/* flusso di esempio */
#include "level7_application.h"
#include <stdio.h>

char* p1 = livello7_send("Ciao!\n");     // incapsulamento ↓
char* m1 = livello7_receive(p1);         // decapsulamento ↑
printf("Ricevuto: %s", m1);
