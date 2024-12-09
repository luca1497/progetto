/*
    Studente:   
    Chiede alla segreteria se ci siano esami disponibili per un corso
    Invia una richiesta di prenotazione di un esame alla segreteria
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include "fullops.h"
#include "common.h"
char *up(char *s){    // up rende le stringhe uppercase
    char *p = strdup(s);  // Duplica la stringa di input in un nuovo buffer
    char *start = p;  // Salva il puntatore all'inizio del buffer
    while (*p != '\0') {
        *p = (char)toupper((unsigned char)*p);
        p++;
    }
    return start;
}
void connettiSegreteria(int *sockfd,struct sockaddr_in *servaddr){
    if ( (*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr,"socket error\n");
        exit (1);
    }
    servaddr->sin_family = AF_INET;
    servaddr->sin_port   = htons(segreteriaPort);
    if (inet_pton(AF_INET, ip, &servaddr->sin_addr) < 0) {
        fprintf(stderr,"inet_pton error for %s\n", ip);
        exit (1);
    }
    if (connect(*sockfd, (struct sockaddr *) &(*servaddr), sizeof(*servaddr)) < 0) {
        fprintf(stderr,"connect error\n");
        exit(1);
    }
    printf("Sono connesso con la segreteria\n");

}
int main(int argc, char **argv)
{

    int                sockfd, n;
    char               buff[maxSize] ;
    char               a;
    Appello            appello;
    Matricola m; 
    RichiestaStudente *richiesta;
    RispostaSegreteria *risposta;
    struct sockaddr_in servaddr;

    printf("Inserire la matricola: ");
    scanf("%s",&m);

    do{
        richiesta = (RichiestaStudente*)malloc(sizeof(RichiestaStudente));
        risposta = (RispostaSegreteria*)malloc(sizeof(RispostaSegreteria));
        // connetto ad ogni azione per motivi di flush: succede che il server non mi risponde perchè
        // ha bufferizzato i dati
        // connettendo e disconnettendo ottengo sempre una risposta
        connettiSegreteria(&sockfd, &servaddr);

        strcpy(richiesta->matricola, m);
        printf("Inserire esame di interesse: ");
        scanf("%s",richiesta->esame);
        strcpy(richiesta->esame, up(richiesta->esame));
        //strcpy(richiesta.esame, buff);
        memset(buff,'\0', sizeof(buff));
        printf("Digitare:\n1 per conoscere la lista delle date di un esame;\n2 per prenotarsi ad un appello.\n3 per uscire\n:  ");
        scanf("%s",&a);
        switch(a){
            case '1':
                strcpy(richiesta->azione, azione1);

                break;
            case '2':
                strcpy(richiesta->azione, azione2);
                break;
            case '3':
                printf("Uscita in corso, arrivederci\n");
                free(risposta);
                free(richiesta);
                close(sockfd);
                return 0;
            default:
            printf("Azione non corretta\n");
            return -1;
        }
        //snprintf(buff, sizeof(buff), "%s\n", "MATEMATICA");
        printf("Esame: %s\n",richiesta->esame);
        //FullWrite(sockfd, esame, strlen(esame));
        printf("Azione: %s\n",richiesta->azione);



        FullWrite(sockfd, (void*)&(*richiesta), sizeof(RichiestaStudente));

        printf("... Aspetto risposta del server ...\n");
        n = FullRead(sockfd, (void*)&(*risposta), sizeof(RispostaSegreteria));
        if(risposta->errore == false){
            switch(a){
                case '1':
                    // ho chiesto la lista di esami
                    printf("Esame: %s -> date: %s\n",risposta->esame, risposta->lista.appello.data);
                break;
                case '2':
                    // voglio prenotarmi ad un appello
                    printf("Prenotato ad appello (materia) %s - (data) %s\n",risposta->prenotazione.appello.esame, risposta->prenotazione.appello.data);
                    break;
                default:
                printf("Azione non corretta\n");
            return -1;
            }
        }else{
            printf("Errore nella richiesta: esame %s non esistente\n",richiesta->esame);
        }
        free(risposta);
        free(richiesta);
        close(sockfd);

    } while(true);

}

