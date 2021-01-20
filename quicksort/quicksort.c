/*
Author: Simon Wibmer
Klasse: 5BHEL
Datum: 20.1.21

Aufgabenstellung: Implementieren Sie Quicksort (https://de.wikipedia.org/wiki/Quicksort, auch Pseudocode ;)) für arrays mit integern.
Testen und Messen sie die Zeiten mit 10, 100, 1000, 10000, 100000 Elementen.

Messungen:
10: 1µs
100: 11µs
1000: 108µs
10000: 2410µs
100000: 225876µs
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>


void swap(int* Swap1, int* Swap2) //Zum Vertauschen von Elementen
{ 
    int temp = *Swap1; 
    *Swap1 = *Swap2; 
    *Swap2 = temp; 
} 
  

int parting (int *a, int us, int os) //Hier wird das Pivot element bestimmt und die Zahlen in Kleiner/Größer geteilt
{ 
int iUnter = us;
int iOber = os - 1;
int iPivot = a[os];
while(iUnter<iOber) //Werte werden durchgezählt falls auf der richtigen Seite -> wird weitergezählt
{
    while ((iUnter<os)&&(a[iUnter]<iPivot)) 
    {
        iUnter++;
    }
    while ((iOber>us)&&(a[iOber]>=iPivot))
    {
        iOber--;
    }
    if (iUnter < iOber)     //falls nicht richtige Seite wird die swap function verwendet um die Elemente zu vertauschen

    {
        swap(&a[iUnter], &a[iOber]);
    }
}
if (a[iUnter] > iPivot)
    {
        swap(&a[os], &a[iUnter]);
    }
return iUnter; 
} 
  
void qs(int *a, int us, int os) // die Quicksort function wird so lange ausgeführt bis if bedingung nicht mehr erfüllt
{ 
    if (us < os) //check ob Obere Grenze größer unterer Grenze ist. Falls nicht -> Fertig
    { 
        int iPivot = parting(a, us, os); 
        qs(a, us, iPivot - 1); //funktion ruft sich selber immmer wieder auf mit verschobenem Pivot element
        qs(a, iPivot + 1, os); 
    } 
} 

// creates a array of size size and fills it with random ints in range 0 to max_int
int *create_array(int size, int max_int)
{
	int *b = (int*)malloc(size * sizeof(int));

	for (int i=0; i<size; i++) {
		b[i] = rand() % max_int;
	}

	return b;
}

#define MY_SIZE 100000

int main(int argc, char **argv)
{
	// create random ints based in current time
	srand(time(NULL));

        int *a = create_array(MY_SIZE, 100);
        struct timeval tv_begin, tv_end, tv_diff;
                gettimeofday(&tv_begin, NULL);
                qs(a, 0, MY_SIZE);
                gettimeofday(&tv_end, NULL);
        timersub(&tv_end, &tv_begin, &tv_diff);
	int old = -1;
	for (int i=0; i<MY_SIZE; ++i)      {
		if (old != -1) assert(old <= a[i]);
		printf("%d ", a[i]);
		old = a[i];
	}
	printf("\n");
    printf("%i elements sorted in %ld seconds %ld microseconds\n", MY_SIZE, tv_diff.tv_sec, tv_diff.tv_usec);
}
