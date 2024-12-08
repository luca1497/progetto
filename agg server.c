#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include "fullops.h"
#include "common.h"


int main(){
    int          listenfd, connfd,n;
    int i = 0;
    struct sockaddr_in  servaddr;
    time_t        ticks;  
    Appello appelli[maxNumAppelli];   // lista di max 100 esami
    Prenotazione prenotazioni[100];
    int numAppelli = 0;
    int numPrenotazioni = 0;
    
    RichiestaSegreteria *richiesta;
    RispostaServer *risposta;

    // inizializzazione strutture del server
    if ( ( listenfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ) {
        perror("socket");
        exit(1);
    }
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(serverPort);
    if ( bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
        perror("bind");
        exit(1);
    }
    if ( listen(listenfd, 1024) < 0 ) {
        perror("listen");
        exit(1);
    }
    for ( ; ; ) {
        printf("in attesa\n");
        if ( ( connfd = accept(listenfd, (struct sockaddr *) NULL, NULL) ) < 0 ) {
            perror("accept");
            exit(1);
        }
        printf("client accettato\n");
        ticks = time(NULL);
        
        /*
          Dopo aver accettato il client, leggo le azioni che vuole che faccia:
            1) Cerca Esami 
            2) Prenota Esami
        */
        richiesta =(RichiestaSegreteria *) malloc(sizeof(RichiestaSegreteria));
        n = FullRead(connfd, &(*richiesta), sizeof(RichiestaSegreteria));

        risposta = (RispostaServer *) malloc(sizeof(RispostaServer));
        risposta->errore = false;
        printf("Tipo di richiesta: %d", richiesta->type);
        if(richiesta->type == 1){
            int i = 0;
            int pos;
            // devo caricare lista esami
            printf("devo caricare %d appelli\n", richiesta->numAppelli);
            while(((numAppelli+i)<maxNumAppelli) && (i < richiesta->numAppelli)){
                memcpy(&appelli[numAppelli+i], &(richiesta->appelli[i]), sizeof(Appello));
                i++;
            }
            numAppelli+=i;
            risposta->numAppelli = i;

            printf("Nuova lista esami (ed appelli): \n");
            for(i = 0; i < numAppelli; i++){
                printf("Appello: %s\n", appelli[i].esame);
            }
        }else if(richiesta->type == 2){
            // devo caricare prenotazione studente
            printf("Prenotazione per: %s ad appello (materia) %s - (data) %s", richiesta->prenotazione.matricola, richiesta->prenotazione.appello.esame, richiesta->prenotazione.appello.data);
            if(numPrenotazioni+1 < 100){
                // permetto il caricamento
                memcpy(&prenotazioni[numPrenotazioni++],&(richiesta->prenotazione),sizeof(Prenotazione));
            }
            printf("Prenotazione completata\n");
        }else{
            printf("Tipo non valido\n");
            risposta->errore = true;
        }

        FullWrite(connfd, (void*)&(*risposta), sizeof(RispostaServer));
        free(risposta);
        free(richiesta);

       

        //
        //if ( write(connfd, buff, strlen(buff)) != strlen(buff)) {
        //    perror("write");
        //    exit(1);
        //}
        close(connfd);
    }
}
