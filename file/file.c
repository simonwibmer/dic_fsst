/*
Author: Simon Wibmer
Klasse: 5BHEL
Jahrgang: 20/21
Abgabedatum: 14.10.20
Github: https://github.com/simonwibmer/dic_fsst/tree/master/file

Aufgabe: Es soll die Funktion cp() mithilfe von POSIX funktionen nachgebaut werden.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#define BSize 20






int copy (char *ifile, char *ofile)
{
    char *buffer[BSize];
    int in,out,rd,wr;

    in = open(ifile, O_RDONLY | O_BINARY);  //open inputfile
    if (in < 0) perror("openin");

    out = open(ofile, O_RDWR | O_TRUNC | O_CREAT | O_BINARY);   //open outputfile
    if (out < 0) perror("openout");
    do
    {
        rd = read(in,buffer,BSize);     //read inputfile
        if (rd  < 0) perror("read");

        wr = write(out,buffer,rd);      //write to outputfile
        if (wr < 0) perror("close");
    } while (rd > 0);

    if (close(in) < 0) perror("closein");
    if (close(out) < 0) perror("closeout");
    return 0;
}


int main (int argc, char* argv[])
{

    copy("input.txt","output.txt");
    return(0);

}