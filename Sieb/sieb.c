/*Author: Simon Wibmer
Funktion: Es soll ein Sieb des Eratosthenes implementiert werden. Dieser hat die Aufgabe aus einem 
bestimmten Zahlenbereich die Primzahlen herauszufinden. Dies wird erreicht indem immer die Vielfachen von
Zahlen gestrichen (0 gesetzt) werden.
Github: https://github.com/simonwibmer/dic_fsst
*/
#include <stdio.h>

int main(void)
{
    int iNumber[501]; //Felddeklaration der Zahlen
    int i; // Zahl von der Vielfaches gebildet werden soll
    int j; //Vielfaches 

    //Füllung des Feldes ############################
    for (i=0;i<=500;i++)
        iNumber[i]=i; 
    //###############################################

    //Herausfinden der Vielfachen####################
    for(j=2;j<=500;j++) //Multiplikation mit jeder Zahl im Bereich (start bei 2 da mult. mit 1 sinnlos)
    {
        for(i=2;j*i<=500;i++) //Zahlen von denen Vielfache gebildet werden sollen werden durchmultipliziert
        {
           iNumber[i*j] = 0;    //diese Zahlen sind keine Primzahlen werden im Feld 0 gesetzt
        }
    }
    //###############################################

    //Ausgabe der Primzahlen#########################
    for(i=2;i<=500;i++) // alle Zahlen die nicht 0 sind werden ausgegeben => start bei 2 da 1 keine Primzahl
    {
        if (iNumber[i] != 0)
        {
            printf("%d\n",iNumber[i]);
        }
    }
    //###############################################
return 0;
}






