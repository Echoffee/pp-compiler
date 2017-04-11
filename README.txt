##########################################################
#      Projet de compilation : compilateur PP > C3A      #
##########################################################

Groupe : Adrien HALNAUT (A4), Yoan MARTY (A1)

L'exécution du Makefile de chaque dossier sans argument génère tous les exécutables nécessaires :
	- ppsyna
	- ppsema
	- ppina
	- ppc3ac
	- c3ai

Tous les exécutables prennent un fichier au format PP ou C3A en entrée et affichent le résultat
sur la sortie d'erreur pour les messages d'états et sur la sortie standard pour donner l'environnement
ou la compilation correspondante.

Question 1 : Analyseur syntaxique PP (ppsyna)
	Lexèmes (flex) : 				ppsyna.l
	Grammaire (bison) : 			ppsyna.y
	Programme principal (AST) : 	ppsyna.c + ppsyna.h
	Vérifie que le fichier corresponde à la grammaire de PP, renvoie une erreur sinon.

Question 2 : Analyseur sémantique PP (ppsema)
	Lexèmes (flex) : 				ppsema.l
	Grammaire (bison) : 			ppsema.y
	Programme principal (AST) : 	ppsema.c + ppsema.h
	Vérifie que le fichier corresponde à la sémantique de PP, renvoie une erreur sinon.

Question 3 : Interpréteur PP (ppina)
	Lexèmes (flex) : 				ppina.l
	Grammaire (bison) : 			ppina.y
	Programme principal (AST) : 	ppina.c + ppina.h
	Retourne l' environnement global (les variables) sur stdout

Question 4 : Compilateur PP > C3A (ppc3ac)
	Lexèmes (flex) : 				ppc3ac.l
	Grammaire (bison) : 			ppc3ac.y
	Programme principal (AST + Liste) : 	ppc3ac.c + ppc3ac.h
	Retourne le programme PP correspondant en C3A sur stdout

Question 5 : Interpréteur c3a (c3ai)
	Programme principal (Listes) : c3ai.l + environ.h + bilquad.h
	Retourne les environnements (et leurs variables) sur stdout

	Le code de iimp est une sorte de fusion des fichiers des questions 3 et 4