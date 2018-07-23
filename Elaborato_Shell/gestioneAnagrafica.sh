#!/bin/bash

file_name="anagrafe.txt"

# Se il primo parametro è diverso da 0 uso il file
# passato durante l'esecuzione, altrimenti quello di default
if test -n "$1"; then  
	file_name=$1
fi
# Creo o aggiorno il file passato come parametro
touch "$file_name"


while true; do
	echo ""
	
	# Definisco il Menu
	echo "Menu:"
	echo "[1] = aggiungi"
	echo "[2] = elimina"
	echo "[3] = visualizza elenco"
	echo "[4] = ricerca"
	echo "[5] = iscritti per anno"
	echo "[6] = esci"
	echo "Inserisci la tua scelta:"

	# Leggo la scelta dell'utente e la salvo nella variabile 'scelta'
	read scelta

	#############################
	# Controllo se 'scelta' == 1
	if test "$scelta" = "1"; then

		echo "Inserire nome:"		
		# Leggo il nome
		read nome									
		echo "Inserire cognome:"
		# Leggo il cognome
		read cognome

		# Inizializzo una variabile di controllo
		matricolaValida=1

		while test "$matricolaValida" -eq "1"; do
			echo "Inserire matricola: "
			read matricola
			# Controllo se matricola è presente nel file
			mat=$(grep -w "$matricola" "$file_name")
			# Verifico se la lunghezza della  matricola è diversa da 6
			if test ${#matricola} != "6"; then
				echo "Matricola non valida."
			# Controllo che la lunghezza della matricola cercata 
			# sia superiore a 0 e quindi già esistente
			elif test -n "$mat"; then
				echo "Matricola gia' esistente."
			else
				matricolaValida=0
			fi
		done

		echo "Inserire anno:"
		# leggo l'anno
		read anno
		# Controllo che l'anno assuma i valori corretti
		while [ "$anno" != "1" ] && [ "$anno" != "2" ] && [ "$anno" != "3" ]; do
			echo "Anno non valido." 
			echo "Inserire anno: "
			read anno
		done								

		# Salvo nel formato "nome;cognome;matricola;anno"
		# appendendo al file tramite l'operatore di redirect output '>>'
		echo "$nome;$cognome;$matricola;$anno" >> $file_name

	###############################
	# Controllo se 'scelta' == 2
	elif test "$scelta" = "2"; then

		echo "Inserire la matricola dello studente che si desidera eliminare:"
		# Leggo la matricola da ricercare
		read matricola			

					
		# Ricerco con 'grep' se dentro il file è presente il pattern $matricola
		ricerca=$(grep -w "$matricola" "$file_name")

		# Controllo se 'studente' ha lunghezza == 0
		if test -z "$ricerca" ; then
			# Ha lunghezza == 0, quindi non c'è stato nessun match nel file
			echo "ERRORE! Non ho trovato: $matricola"
		else

			while read riga; do
			    # Stampo la riga e con 'grep' ricerco la matricola inserita
				cerca_matricola=$(echo "$riga" | grep -w "$matricola") 

				if test -z "$cerca_matricola" ; then
					# Se non ho trovato la matricola ricercata 
					# appendo al file anagrafe1 il contenuto di riga
					echo "$riga" >> anagrafe1.txt
				fi
			# Legge le righe da file_name
			done < "$file_name"
			# Sovrascrivo con > il contenuto di anagrafe1 nella variabile file_name
			cat anagrafe1.txt > "$file_name"  
			rm anagrafe1.txt

		fi

	###############################
	# Controllo se 'scelta' == 3
	elif test "$scelta" = "3"; then
		# Creo la variabile tabella riordinando per matricola
		# e uso come delimitatore il ';'
		tabella=$(sort -d -k3 -t ';' "$file_name")
		echo "$tabella" > tab.txt
		echo "NOME COGNOME MATRICOLA ANNO"
			
			while read riga; do
				# Definisco le variabili da stampare selezionando le singole colonne
				nome=$(echo "$riga" | cut -d ';' -f1)
				cognome=$(echo "$riga" | cut -d ';' -f2)
				matricola=$(echo "$riga" | cut -d ';' -f3)
				anno=$(echo "$riga" | cut -d ';' -f4)

				# Stampo sulla singola riga le variabili definite in precedenza
				echo -e "$nome\t$cognome\t$matricola\t$anno"

			# Continuo il ciclo finchè non ho letto tutte le righe
			done < "tab.txt"
			rm tab.txt


	###############################
	# Controllo se 'scelta' == 4
	elif test "$scelta" = "4"; then
		echo "Inserire il cognome dello studente che si vuole ricercare: "
		read cognome

		#nome;cognome;matricola;anno
		# grep -w viesi
		# grep -w ";cognome"

		# Cerco il cognome all'interno del file
		temp=";"$cognome; 
		cerca_cognome=$(grep -e "$temp" anagrafe.txt)
		# Se la variabile è vuota il cognome non è presente
		if test -z "$cerca_cognome" ; then

			echo "ERRORE! Non ho trovato: $cognome"

		else 
			# Ordino in ordine alfabetico la riga in cui è presente il cognome 
			ordina=$(grep -e "$temp" anagrafe.txt | sort -d)
			echo "$ordina" > elenco.txt
			while read riga; do
				# Definisco le variabili da stampare selezionando le singole colonne
				echo "Nome: $(echo "$riga" | cut -d ';' -f1)"
				echo "Cognome: $(echo "$riga" | cut -d ';' -f2)"
				echo "Matricola: $(echo "$riga" | cut -d ';' -f3)"
				echo "Anno: $(echo "$riga" | cut -d ';' -f4)"
				echo ""

			# Continuo il ciclo finchè non ho letto tutte le righe
			done < "elenco.txt"	
			rm elenco.txt

		fi
		
	###############################
	# Controllo se 'scelta' == 5
	elif test "$scelta" = "5"; then
		# Definisco il delimitatore ';' nella quarta colonna 
		# Conto quante volte ho il match del numero ricercato nel file 
		cerca_uno=$(cut -d ';' -f4 "$file_name" | grep -c '1')
		cerca_due=$(cut -d ';' -f4 "$file_name" | grep -c '2')
		cerca_tre=$(cut -d ';' -f4 "$file_name" | grep -c '3')

		echo "Primo anno: $cerca_uno"
		echo "Secondo anno: $cerca_due"
		echo "Terzo anno: $cerca_tre"

	##############################
	# Controllo se 'scelta' == 6
	elif test "$scelta" = "6"; then
			echo "TERMINAZIONE IN CORSO"
			exit

	##############################
	else
		echo "Scelta non valida."
	fi

done


# VR414213
# Eva Viesi
# 07/05/2018
# gestioneAnagrafica