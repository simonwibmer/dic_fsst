/*Author: Simon Wibmer
Funktion: Es sollen ganzzahlige Wurzeln aus pos int Zahlen berechnet werden, bei nicht ganzzahlen -> abrunden
Die hierfür zu implementierende Funktion darf nur die operanden: ++, !=, <= enthalten; Es dürfen alle Schleifentypen verwendet werden
*/
#include <stdio.h>

int quadrat (int iSqrtUebergabe){ //funktion um Zahlen zu Quadrieren; zwei verschachtelte Schleifen mit der angegebenen Zahl als vergleich zur Laufvariable
    int i=0;
    i++;
    int iQuadriert=0;
    for(; i<=iSqrtUebergabe; i++ ){ //Erste Schleife entspricht dem ersten teil der Multiplikation mit sich selbst (z.b. 4x4 entspricht der ersten 4)
        int j=0;
        j++;
        for(; j<=iSqrtUebergabe; j++){ //Zweite Schleife entspricht der zweiten Zahl in der Multiplikation
            iQuadriert++;              // Hier wird das Ergebnis mitgezählt
            
        }

    }
    return iQuadriert;
}

int my_sqrt(int iInput){
    int isqrtZahl = 0;
    int isqrtZahlfinal = -1;
    if (iInput<=-1){
        printf("Nur positive Zahlen sind erlaubt!");  
    }
    else{

    while(quadrat(isqrtZahl)<=iInput){   //Es werden (start bei 0) in 1er Schritten Zahlen quadriert und mit dem gewünschten verglichen
                                        //übersteigt das quadratsergebnis die gewünschte Zahl muss die Wurzel dieser die vorherige quadratzahl sein 
        isqrtZahl++;                    //Zahlen die Quadriert werden
        isqrtZahlfinal++;               //Zahlen die Quadriert werden -1; entspricht dem Wurzelergebnis
        
    }
    printf("Wurzel der gewünschten Zahl:%i",isqrtZahlfinal);
    }
}


int main(void){
    my_sqrt(16);
    return 0;
}