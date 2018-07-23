#include "defines.h"
/* funzioni del padre */ 
void sigalarm_handler(int signal);
void wait_child_ended();
void send_message_to_childs(int n_op_per_figlio[MAX_CHILD]);
void terminate();
void save_output(char* f_path);
void read_file_and_fill_matrix(int data_readed, int fd, int *n_figli_da_generare, int n_op_per_figlio[]);
