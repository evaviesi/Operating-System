#!/bin/bash

echo "rimuovo la coda di messaggi"
ipcrm -Q 777777

echo "rimuovo il file output.txt"
rm output.txt

echo "compilo ed eseguo"
gcc -o elaborato include/defines.h src/figlio.c include/figlio.h src/helper.c include/helper.h include/mytypes.h src/padre.c include/padre.h
./elaborato