all : lexical syntaxique semantique compile Executeur
.PHONY : clean

lexical: analyse_lexicale.c
	gcc -Wall -o lexicale analyse_lexicale.c

syntaxique: analyse_syntaxique.c
	gcc -Wall -o syntaxique analyse_syntaxique.c

semantique: analyse_semantique.c
	gcc -Wall -o semantique analyse_semantique.c

compile: Compilateur.c
	gcc -Wall -o compile Compilateur.c

Executeur: Executeur.c
	gcc -Wall -o Executeur Executeur.c

clean:
	rm lexicale
	rm syntaxique
	rm semantique
	rm compile
	rm Executeur