#include <stdio.h>

void main()
{
    int iNumber[500]; //Felddeklaration
    int i;

    
    for (i=1;i<=500;i++)
        iNumber[i]=i; //Feld füllen
    
    int j;
    for(j=2;j<=500;j++)
    {
        for(i=2;j*i<=500;i++)
        {
           iNumber[i*j] = 0;
        }
    }
    
    for(int i=2;i<=500;i++)
    {
        if (iNumber[i] != 0)
        {
            printf("%d\n",iNumber[i]);
        }
    }
}





