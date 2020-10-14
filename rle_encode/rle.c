#include <stdio.h>

#define MAX_RLEN 10

char *encode (char *string)
{

}



int main(int argc, char **argv)
{
	char str[MAX_RLEN];
      
       	do {	
		if (fgets(str, MAX_RLEN, stdin) == NULL) break;
		str[MAX_RLEN] = 0;
		str[strlen(str)-1] = 0;
		char* res = encode(str);
		printf(">%s\n", res);
		free(res);
	} while (strlen(str) > 1);
}