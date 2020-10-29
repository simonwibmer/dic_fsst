/*
Author: Simon Wibmer
Klasse: 5BHEL
Jahrgang: 20/21
Abgabedatum: 14.10.20
Github:https://github.com/simonwibmer/dic_fsst/tree/master/rle_encode

Aufgabe: Die Funktion encode() aus einer Vorgegebenen main (moodle kurs) schreiben. Diese soll erhaltene
Strings auswerten und ausgeben wie oft jeder gleiche Buchstabe hintereinander vorkommt. Also aus 'Hallo' -> 'H1a1l2o1'
*/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RLEN 15

char* encode(char* str)
{
	int iMenge = 1;
	int i,j=0;
	char *ausgabe = malloc(MAX_RLEN);
	char *ablage = malloc(4);

	for(i=0 ;i <=strlen(str)-1;i++)
	{
		//Zaehlen wie oft die gleiche Zahl vorkommt
		if(str[i]==str[i+1])
		{
			iMenge++;
		}	
		else
		{
			//Zusammensetzen von Buchstabe und der Anzahl; abgelegt in ablage
			sprintf(ablage,"%c%d",str[i],iMenge);
			*(ausgabe+j++) = *(ablage); //Buchstabe wird dem Ergebnis aus der Ablage zugewiesen
			*(ausgabe+j++) = *(ablage+1); // Anzahl wird dem Ergebnis aus der Ablage zugewiesen
			if(iMenge>=10)
			{
				*(ausgabe+j++) = *(ablage+2); // Bei zweistelligen anzahlen muss noch ein platz im Ergebnis verwendet werden
			}
			iMenge=1;
		}
	}
	
	*(ausgabe+j) = ('\0'); // Nullterminierung
	free(ablage);			//"löschen" der Ablage
	return(ausgabe);
}

//#########################################################
int main(int argc, char **argv)
{
	char str[MAX_RLEN];
    do
	{	
		if (fgets(str, MAX_RLEN, stdin) == NULL) break;
		str[MAX_RLEN] = 0;
		str[strlen(str)-1] = 0;
		char* res = encode(str);
		printf(">%s\n", res);
		free(res);
	}
	while (strlen(str) > 1);
}