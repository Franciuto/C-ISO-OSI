/* C-ISO-OSI - Main (test) */
/* flusso di esempio */
char* p1 = livello7_send("Ciao!\n");     // incapsulamento ↓
char* m1 = livello7_receive(p1);         // decapsulamento ↑
printf("Ricevuto: %s", m1);
