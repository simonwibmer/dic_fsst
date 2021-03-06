/*
Author: Simon Wibmer
Klasse: 5BHEL
Abgabedatum: 2.12.20
Github: https://github.com/simonwibmer/dic_fsst/tree/master/binary
Aufgabe:
-Spiele http://20q.net/.
-Verstehe: https://de.wikipedia.org/wiki/Bin%C3%A4re_Suche
-Implementiere rekursive binäre Suche in C, benutze die Funktion strcmp(3)
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>

#define Buffer_Size 50000000

//################################################################
char** binary(char* Input, char** search_index,int Ug, int Og)
{
    int stringreturn = strcmp(Input,*(search_index+(Og+Ug)/2));     //strcmp zwischen dem gesuchten Wort und der Wortliste an der mittlersten Stelle
    if (Ug == Og || Ug == (Og+Ug)/2)      //Abbruch Bedingung
    {
        return NULL;
    }
    if(stringreturn < 0)                // Wenn stringreturn kleiner Null -> gesuchtes Wort im unteren Teil der Liste
    {
        Og = (Ug+Og)/2;                 // Obergrenze wird auf das vorherige Mittel gesetzt
    }
    else if(stringreturn > 0)           //Wenn stringreturn gößer Null -> gesuchtes Wort om oberen Teil der Liste
    {
        Ug = (Ug+Og)/2;                 //Untergrenze wird auf das vorherige Mittel gesetzt
    }
    else                                // stringreturn == 0 -> Wort gefunden
    {
        printf("Wort gefunden: %s an Stelle %i\n", *(search_index+((Ug+Og)/2)),(Ug+Og)/2);
        return (search_index+((Ug+Og)/2));
    }
     
    return binary(Input,search_index,Ug,Og); // kommt das Programm hier an wurde das Wort noch nicht gefunden -> 
                                            //Es wird die Funktion nocheinmal mi den veränderten grenzen returned
}
//##############################################################################
char** searchIx(char* Input)
{
    
    char* Buffer=malloc(Buffer_Size); // Speicherbereich allocation für den Buffer
    int i = 0;

    int op= open("wortbuffer", O_RDONLY);       //Öffnen und lesen des Wordbuffers
    if (op < 0) perror("open");
    int rd = read(op, Buffer, Buffer_Size);
    if (rd  < 0) perror("read");
    
    char** search_index=malloc(rd);                 // search_index wird definiert -> Speicherbereich allocation
    search_index[i] = Buffer;                      // Übergeben des ersten Wordes von Buffer in die 0 Stelle von search_index

    for(int x=0; x<rd; x++)                         //for Schleife läuft das gesamte Dokument durch
    {
        if (*(Buffer+x) == 0)                       // bei einer 0 im Buffer wird das Wort in search_index geschrieben und der Buffer erhöht
        {
            search_index[++i] = Buffer+(++x);
        }
    }
   return binary(Input, search_index,0,i);
}
//###########################################################################
int main(int argc, char** argv)
{

    for (;;) {
                char input[100];
                fgets(input, sizeof(input), stdin);
                input[strlen(input)-1] = 0;
                if (!strlen(input)) break;

                struct timeval tv_begin, tv_end, tv_diff;
                gettimeofday(&tv_begin, NULL);
                void *res = searchIx(input);
                gettimeofday(&tv_end, NULL);

                timersub(&tv_end, &tv_begin, &tv_diff);

                if (res != NULL) {
                        printf("found\n");
                } else {
                        printf("not found\n");
                }
                printf(" in (%ld seconds %ld microseconds)\n", tv_diff.tv_sec, tv_diff.tv_usec); 
        }


    return 0;
}