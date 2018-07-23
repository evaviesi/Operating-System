/*
	Compilazione:
	>gcc -o elaborato include/defines.h src/figlio.c include/figlio.h src/helper.c include/helper.h include/mytypes.h src/padre.c include/padre.h

	Lancio:
	>./elaborato

*/


// librerie std
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

// per la wait()
#include <sys/types.h>
#include <sys/wait.h>

// system call coda messaggi
#include <sys/ipc.h>
#include <sys/msg.h>

// figlio
#include "../include/figlio.h"

// msg queue declaraation
#include "../include/mytypes.h"

// funzioni padre
#include "../include/padre.h"

// defines
#include "../include/defines.h"

// helper functions
#include "../include/helper.h"


// GLOBALS VAR
int    		  matrix[M_ROW][M_COL] = {0}; /* matrice contentente le operazioni da eseguire */
pid_t  		  child_pid[9]	       = {0}; /* array contentente pid figli */
int    	      n_figli_da_generare  =  0;  /* # figli da generare */
struct msgbuf msg;						  /* struct messaggio per la coda dei messaggi*/
int	   		  msg_id;				      /* msg queue id */


int main (int argc, char *argv[])
{	

	
	int    fd; 						 		  /* file descriptor*/
	int    data_readed;				 		  /* ritorno della read*/
	char   buf[BUFFER_SIZE]; 		 		  /* buffer di lettura/scrittura*/
	int    p[10][2];				 		  /* pipe tra padre e figlio*/
	int    n_op_per_figlio[MAX_CHILD] = {0};  /* calcolo quante op ho per figlio rimanenti */	
	int    res[M_ROW][11] 			  = {0};

	/*creo la coda di messaggi*/
	if((msg_id = msgget(777777, (0666|IPC_CREAT|IPC_EXCL))) == -1 ) {
		perror("Errore nella creazione della coda di messaggi\n");
		exit(1);
	}

	/* apertura del file da leggere */
	if ((fd = open("config.txt",O_RDONLY, S_IRUSR)) == -1) {
		perror("File non apribile.");
		terminate();
	}

	/* lettura file */
	read_file_and_fill_matrix(data_readed, fd, &n_figli_da_generare, n_op_per_figlio);

	/* intercetto il segnale della SIGALARM */
	signal(SIGALRM, sigalarm_handler);

	/* lancio tutti i fgli */ 
	pid_t ppid= getpid();
	int i;
	for(i=0;i<n_figli_da_generare;i++) {

		/* instanzo la pipe i^esima */
		if(pipe(p[i+1]) < 0) {
			perror("errore pipe");
			terminate();
		}

		/* genero il figlio i^esimo */
		if((child_pid[i] = fork()) == -1) {
			perror("fork fallita");
			terminate();
		}
		else if (child_pid[i] == 0 ) {

			/* lancio il wrapper del figlio */
			figlio(p[i+1], i+1, msg_id);
			break;
		}
	}

	/* solo il padre entra qua */
	if(ppid == getpid()) {

		/* invio nella coda dei messaggi le operazioni */
		send_message_to_childs(n_op_per_figlio);

		/* mi metto in ascolto con la read sui figli */
		int i;
		for(i=0;i<n_figli_da_generare;i++) {

			/* setto timeout a 10 s */
			alarm(10);

			/* array dei risultati */
			int res[10] = {0};

			char buf[512];
			/* leggo dalla pipe i risultati */
			int byte_read = read(p[i+1][0], buf,512);
	
			/* conto quanti spazi ho */
			int count_space = 0;
			int k;
			for(k = 0; k < byte_read; k++) {
				if(buf[k] == ' ')
					count_space++;
			}

			/* ho n_spazi + 1 risultati da elaborare, mi preparo l'array: so che ho massimo 10 cifre per calcolo */
			int row = 0;
			k = 0;
			for(k = 0; k < byte_read; k++) {
				
				int tmp = k;
				int digits = 0;
				while(buf[k] != ' ') {
					k++;
					digits++;
				}

				if(digits < 10) {
					int decimal = 1;

					while(k > tmp) {
						k--;
						if((buf[k]) == '-') {
							res[row] = -res[row];

						}
						else
						{
							res[row] += ((buf[k] - 48) * decimal);
							decimal*=10;
						}
					}


					row++;
					k = k + digits;
				}
			}

			int m_row;
			for(m_row=1;m_row<=M_ROW;m_row++) {
				/* ho corrispondenza nella matrice, quindi salvo il prossimo valore di res[] */
				if( matrix[m_row][0] == (i+1) && row > 0) {
					matrix[m_row][4] = res[count_space - row];
					row--;
				}
			}
		}  

		
		/*salvo l'output*/
		/*
			Se voglio passare il parametro del file output diventa:
			save_output(argv[1]);
			*argv[0] --> elaborato (aka ./elaborato || a.out)
			*argv[1] --> primo parametro (file_name)	
		*/

		save_output("output.txt");

		/* termino e libero le risorse */
		terminate();

	}
}

void read_file_and_fill_matrix(int data_readed, int fd, int *n_figli_da_generare, int n_op_per_figlio[]) {
	char buf[BUFFER_SIZE];
	
	/* lettura file */
	while ((data_readed = read(fd,buf,BUFFER_SIZE)) > 0)
    {
		int row   = 1;
		int blank = 0;

		
		/* riempio la matrice di appoggio */
		int i;
		for(i=2 ; i < data_readed - 1 ;i++) {

			/* trovo il primo spazio */
			if(buf[i] == ' ' && blank==0) {
				matrix[row][0]=(buf[i-1]-48);
				int numb_lenght = 0;
				i++;

				while(buf[i]!=' ') {
					numb_lenght++;
					i++;
				}

				i = i-numb_lenght;

				int decimal = 1;
				int k;
				for( k = numb_lenght; k > 0; k--) {
					matrix[row][1]+=(buf[(k+i) -1]-48) *decimal;
					decimal *= 10;
				}
				blank++;
			}

			/* trovo il terzo spazio */
			if(buf[i] == ' ' && blank==2) {
				int numb_lenght = 0;

				i++;
				while(buf[i]!='\n') {
					numb_lenght++;
					i++;
				}

				i = i- numb_lenght;

				int decimal = 1;
				int k;
				for( k = numb_lenght; k > 0; k--) {
					matrix[row][3]+=(buf[(k+i) -1]-48) *decimal;
					decimal *= 10;
				}
				blank++;
			}

			/*  trovo il secondo spazio */
			if(buf[i] == ' ' && blank==1) {
				matrix[row][2] = char_to_operation(buf[i+1]);
				blank++;
			} 

			/* nuova riga */
			if(buf[i] == '\n') {
				row++;
				blank = 0;
			}	
		}

		/* calcolo il numero di figli da generare */
		*n_figli_da_generare = buf[0] - 48;

		/* calcolo quante operazioni sono destinate al figlio */
		int tmp;
		int j;
		for(j=0; j<data_readed-1 ; j++) {
			if(buf[j]=='\n') {
				tmp=buf[j+1]-48;
				n_op_per_figlio[tmp-1]++;
						
			}
		}	
	}
}

/* avviso della scadenza del tempo */
void sigalarm_handler(int signal) {
	printf("\n\n\nTempo scaduto!\n\n\n");
	
	/* termino e libero le risorse */
	terminate();
}


/* invio le operazioni ai figli tramite al coda di messaggi */
void send_message_to_childs(int n_op_per_figlio[MAX_CHILD]) {
	int i;
	for(i=0;i<=M_ROW;i++) {	

		struct msgbuf message; 	
		
		/* controllo a quale figlio è destinato il  messaggio e decremento
		le operazioni che quest'ultimo deve svolgere*/
		if(matrix[i][0]!=0) 
		{
			/* mtype corrisponde all'id del figlio */
			message.mtype=matrix[i][0];

			if(n_op_per_figlio[(matrix[i][0])-1] > 0) 
			{
				n_op_per_figlio[(matrix[i][0])-1]--;
		
				message.op1        = matrix[i][1];
				message.operazione = matrix[i][2];
				message.op2		   = matrix[i][3];
				message.termine    = '0';

				int res_msg = msgsnd(msg_id, &message, sizeof(struct msgbuf), 0);
				if(res_msg==-1) {
					perror("Errore nell'invio della coda di messaggi\n");
					terminate();
				}

			}
			/* se le operazioni da svolgere sono terminate invio la X */
			if(n_op_per_figlio[(matrix[i][0])-1] == 0 ) 
			{
				message.op1        = -1 ;
				message.operazione = -1 ;
				message.op2        = -1 ;
				message.termine    = 'X';

				int res_msg = msgsnd(msg_id, &message, sizeof(struct msgbuf), 0);
				if(res_msg==-1) {
					perror("Errore nell'invio della coda di messaggi\n");
					terminate();
				}

			} 
		}		
	}
}

/* attendo che i figli abbiano terminato */
void wait_child_ended() {
	int i;
	for(i=0;i<n_figli_da_generare;i++) {
		wait(&child_pid[i]);
	}
}

/* salvo l'ouput */
void save_output(char* f_path) {
	int row;
	int fd;
	char output[50]={' '};
	

	/* creo il file di output */
	fd = open(f_path, O_WRONLY|O_CREAT, 0640);
		if (fd==-1) {
			perror("Errore nella creazione del file");
			exit(1);
		}


	for(row=1;row<=M_ROW;row++) {
		if(matrix[row][0] == 0) 
			break; 
		
		
		int operando1 = matrix[row][1];
		int operando2 = matrix[row][3];
		int risultato = matrix[row][4];
		char operazione = operation_to_char(matrix[row][2]);

		/* calcolo la dimensione dell'array di output, dimensione interi + 5 spazi + \n */
		int dim_output=integerLength(operando1)+integerLength(operando2)+integerLength(risultato)+6;

		/* aggiungo 1 alla dimensione se il numero è negativo (-) */
		if(operando1<0) {
			dim_output++;
		}
		if(operando2<0) {
			dim_output++;
		}
		if(risultato<0) {
			dim_output++;
		}

		/* stampo nell'array output la riga che scriverò nel file di output */
		snprintf(output, dim_output+1, "%d %c %d = %d", operando1, operazione, operando2, risultato);
		write(fd, output, dim_output);
		write(fd, "\n", 1);
		
	}	

	close(fd);

	// stampo la matrice debug
	/*
	int col;
	for(row=1;row<=M_ROW;row++) {
		if(matrix[row][0]==0)
			break;
		for(col = 0; col < M_COL; col++) {
			printf("[%d]", matrix[row][col]);
		}	
		printf("\n");
	}
	printf("\n");
*/

}

/*  wapper funzioni di terminazioni */
void terminate() {

	/* avvio la terminazione:
	 invio la SIGTERM a tutti i figli e libero le risorse */
	int i;
	for(i=0;i<n_figli_da_generare;i++) {
		kill(child_pid[i], SIGTERM);
	}

	/* aspetto che abbiano finito tutti */
	wait_child_ended();

	/* rimuovo la coda di messaggi */
	msgctl(msg_id, IPC_RMID, NULL);

	exit(0);
}

