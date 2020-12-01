/* 
Author: Simon Wibmer
Klasse 5BHEL
Abgabedatum: 25.11.20
Aufgabe: Schreiben Sie einen TCP Server Prozess der alle vom Client geschickte Daten zurück sendet. 
Verwenden Sie die POSIX Funktionen (s.a. https://de.wikipedia.org/wiki/Socket_(Software) ): bind(2), listen(2), accept(2), recv(2), send(2).
Als Bonusaufgabe: Geben sie den Inhalt RLE encoded zurück.
Anmerkung: Es wurde einiges von diesem Code aus den Beispielen der MAN-Pages und Wikipedia entnommen
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>  //includes für die verschiedenen funktionen; aus den jeweiligen Man-pages
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>         
#define ADDR "127.0.0.1"  //defines für Adresse, Port und Buffersize
#define PORT 60000
#define BSize 32

#define handle_error(msg) \
    do {perror(msg); exit(EXIT_FAILURE);} while (0) //kommt von den Man-Pages für bind(), definiert handle_error

int server ()
{
    char* buffer=malloc(BSize);     //buffer zuweisung

    struct sockaddr_in adresse;     //deklarierung Socket-Adressstruktur 
    socklen_t adresse_size;        

    memset(&adresse,0,sizeof(struct sockaddr_in)); //füllt den gesmaten Memorybereich von adresse mit 0 

    adresse.sin_family = AF_INET;               //IPV4
    adresse.sin_port = htons(PORT);             //Portzuweisung
    adresse.sin_addr.s_addr = inet_addr(ADDR);  //IP-Adressen zuweisung
    

    int sockfd = socket(AF_INET, SOCK_STREAM,0);    //socket wird erstellt
    if (sockfd == -1){
        handle_error("socket");
        close(sockfd);
    }

    if (bind(sockfd,( struct sockaddr *) &adresse, sizeof(struct sockaddr_in))== -1){ //weist dem Socket eine Adresse zu
        handle_error("bind");
        close(sockfd);
    };

    if(listen(sockfd,5) == -1){  //Socket wartet auf Verbindungen, "Verbindungswarteschlange" beträgt 5 
        handle_error("listen");
        close(sockfd);
    };

    int clientfd = accept(sockfd, &adresse, &adresse_size); //akzeptiert eine Verbindung
    if (clientfd == -1){
        handle_error("accept");
        close(sockfd);
    }

    while(2602){                                //infinite loop
        int re = recv(clientfd,buffer,BSize,0);     //nachricht wird gelesen und in den buffer geschrieben
        if(re == -1)
        {
            handle_error("recieve");
            close(sockfd);
        }
        buffer[re] = 0;
        int se = send(clientfd, buffer,strlen(buffer),0);   //nachricht wird aus dem Buffer zurückgesendet

        if (se == -1)
        {
            handle_error("send");
            close(sockfd);
        }
    }

}

int main(){
    server();
}