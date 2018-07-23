// librerie std
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

// manipolazione stringhe
#include <string.h>

// per la wait()
#include <sys/types.h>
#include <sys/wait.h>

// system call coda messaggi
#include <sys/ipc.h>
#include <sys/msg.h>

//functions
#include "../include/figlio.h"

// message queue definition
#include "../include/mytypes.h"

// defines
#include "../include/defines.h"

/* funzione per la computazione delle operazioni */
int make_operations(struct msgbuf message);

void figlio(int p[], int n_figlio, int msg_id_rcv) {

    int  risultati[MAX_CHILD] = {0};

    /* array di strutture contenente le operazioni */
    struct msgbuf arr_operazioni[MAX_CHILD];

    /* buffer operazioni: (10 cifre max per numero * 10 calcoli) + 9 spazi + \0 */
    char   buffer_operazioni_separate_da_spazio[(MAX_DIGIT * 10) + (9) + 1] = "";

    /* mi prenoto per i segnali */
    signal(SIGTERM, sigterm_handler);
 	
    /* cerco all'infinito il numero del figlio, finchè non arriva la X */
    int i = 0;
    while(1) { 
        i++;
        struct msgbuf message;

        /* leggo il messaggio inviato dal padre dalla coda di messaggi */
        if ( (msgrcv(msg_id_rcv, &message, sizeof(struct msgbuf), n_figlio, 0)) < 0 ) {
            printf("errore nella msg_rcv [figlio.c]");
        }

        if(message.termine=='X') {
            /* computo le operazioni */
            int j;
            for(j = 1; j < i; j++) {
                risultati[j] = make_operations(arr_operazioni[j]);

                char output[10];
                /* stampa rislultati[j] dentro la stringa output lunga 10 */
                snprintf(output, 10, "%d", risultati[j]); 

                /* concateno la stringa output con lo spazio per separare i risultati del figlio */
                strcat(buffer_operazioni_separate_da_spazio, output);
                strcat(buffer_operazioni_separate_da_spazio, " ");
            }

            /*  scrivo sul canale della pipe la stringa dei risultati */
            if( (write(p[1], &buffer_operazioni_separate_da_spazio, ((MAX_DIGIT * 10) + (9) + 1))) < 0) {
                perror("Error write figlio\n");
                exit(1);
            }

            exit(0);
        }
        else 
        {
            /* salvo le operazioni in un array di struct */
            arr_operazioni[i].op1        = message.op1;
            arr_operazioni[i].operazione = message.operazione;
            arr_operazioni[i].op2        = message.op2;
        }
    }
}

void sigterm_handler(int sig) {
    /* termino se ho intercettato il segnale */
    printf("Ho intercettato la SIGTERM\n");
    exit(0);
}

/* converto le operazioni ricevute computandole */
int make_operations(struct msgbuf message) {
    switch(message.operazione) {
        case 1: return message.op1 + message.op2;
        break;
        case 2: return message.op1 - message.op2;
        break;
        case 3: return message.op1 * message.op2;
        break;
        case 4: return message.op1 / message.op2;
        break;
        default: return -1; 
    } 
}
