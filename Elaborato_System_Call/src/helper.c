#include "../include/helper.h"
#include <stdio.h>

/* converto le operazioni in numeri interi */
int char_to_operation(char value) {
 	switch(value) {
		case '+': return 1; break;
		case '-': return 2; break;
		case '*': return 3; break;
		case '/': return 4; break;
		default: return -1;
 	}
}

/* converto le operazioni in numeri interi */
int operation_to_char(int value) {
 	switch(value) {
		case 1: return '+'; break;
		case 2: return '-'; break;
		case 3: return '*'; break;
		case 4: return '/'; break;
		default: return '#';
 	}
}

/* calcolo la lunghezza di un numero intero */
int integerLength(int value) {
    int count = 0;
	value=negative_integer(value);
	if(value==0) return 1;

    while(value > 0) {
        count ++;
        value = value /10;
    }

    return count;
}   

/* restituisco l'intero con il meno davanti */
int negative_integer(int value) {
	if(value<0) {
		return -value;
	}
}
