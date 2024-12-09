/*
    Segreteria:
    Inserisce gli esami sul server dell'università (salvare in un file o conservare in memoria il dato)
    Inoltra la richiesta di prenotazione degli studenti al server universitario
    Fornisce allo studente le date degli esami disponibili per l'esame scelto dallo studente

    Segreteria, quindi, si comporterà come un server per Studente e come un client per Server Universitario,
    bisogna allocare le strutture dati per entrambi i ruoli.


*/

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





void inizializzaEsami(Appello *appelli, int *numEsami){
    strcpy(appelli[0].esame, "STORIA\0");
    strcpy(appelli[0].data, "10-12-24\0");

    strcpy(appelli[1].esame, "MATEMATICA\0");
    strcpy(appelli[1].data, "09-01-24\0");

    strcpy(appelli[2].esame, "GEOGRAFIA\0");
    strcpy(appelli[2].data, "07-08-24\0");

    strcpy(appelli[3].esame, "ANALISI1\0");
    strcpy(appelli[3].data, "01-09-24\0");

    strcpy(appelli[4].esame, "PALEOLOGIA\0");
    strcpy(appelli[4].data, "07-01-24\0");

    strcpy(appelli[5].esame, "ANALISI2\0");
    strcpy(appelli[5].data, "13-08-24\0");

    *numEsami = 6;

}


void inviaEsamiAlServer(int socket, Appello appelli[maxNumAppelli], int numAppelli){
    int i;
    printf("Invio gli esami al server\n");
    RichiestaSegreteria * richiesta = (RichiestaSegreteria *)malloc(sizeof(RichiestaSegreteria));
    RispostaServer * risposta = (RispostaServer *)malloc(sizeof(RispostaServer));

    richiesta->numAppelli = numAppelli;
    richiesta->type = 1;
    memcpy(richiesta->appelli, appelli, sizeof(Appello)*maxNumAppelli);
    FullWrite(socket, (void*)&(*richiesta), sizeof(RichiestaSegreteria));
    FullRead(socket, &(*risposta), sizeof(RispostaServer));
    if(risposta->errore == 0){
        printf("Caricati %d esami\n", risposta->numAppelli);
    }else{
        printf("Errore caricamento esami\n");
    }
    free(richiesta);
    free(risposta);
    
}

void inviaPrenotazione(int socket, Appello appello, Matricola matricola){
    printf("Matricola studente: %s\n", matricola);
    printf("Inoltro prenotazione al server\n");
    RichiestaSegreteria * richiesta = (RichiestaSegreteria *)malloc(sizeof(RichiestaSegreteria));
    RispostaServer * risposta = (RispostaServer *)malloc(sizeof(RispostaServer));

    richiesta->type = 2;
    memcpy(&(richiesta->prenotazione.appello), &appello, sizeof(Appello));
    memcpy(&(richiesta->prenotazione.matricola), matricola, sizeof(Matricola));

    printf("materia: %s, data: %s, matricola: %s\n", richiesta->prenotazione.appello.esame, 
    richiesta->prenotazione.appello.data,
    richiesta->prenotazione.matricola);

    FullWrite(socket, (void*)&(*richiesta), sizeof(RichiestaSegreteria));
    FullRead(socket, &(*risposta), sizeof(RispostaServer));
    printf("errore: %d\n", risposta->errore);
    if(risposta->errore == 0){
        printf("Inviata prenotazione di matricola %s per appello (materia) %s - (data) %s",
        richiesta->prenotazione.matricola, richiesta->prenotazione.appello.esame, 
        richiesta->prenotazione.appello.data );
    }else{
        printf("Errore invio prenotazione\n");
    }
    free(richiesta);
    free(risposta);

}

int cercaEsame(Esame e, Appello *appelli, int numEsami){
    bool trovato = false;
    int i=0, pos = -1;
    while((i<numEsami) && (trovato == false)){
        if(strcmp(e, appelli[i].esame) == 0){
            trovato = true;
            pos = i;
        }
        i++;
    }
    return pos;
}

void connettiAlServer(int *sockfd,struct sockaddr_in *servaddr){
    if ( (*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr,"socket error\n");
        exit (1);
    }
    servaddr->sin_family = AF_INET;
    servaddr->sin_port   = htons(serverPort);
    if (inet_pton(AF_INET, ip, &servaddr->sin_addr) < 0) {
        fprintf(stderr,"inet_pton error for %s\n", ip);
        exit (1);
    }
    if (connect(*sockfd, (struct sockaddr *) &(*servaddr), sizeof(*servaddr)) < 0) {
        fprintf(stderr,"connect error\n");
        exit(1);
    }
    printf("Sono connesso con il server universitario\n");

}

void initSegreteria(int *sockfd, struct sockaddr_in *servaddr){
    // inizializzazione strutture del server
    if ( ( *sockfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ) {
        perror("socket");
        exit(1);
    }
    servaddr->sin_family   = AF_INET;
    servaddr->sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr->sin_port        = htons(segreteriaPort);
    if ( bind(*sockfd, (struct sockaddr *) &(*servaddr), sizeof(*servaddr)) < 0 ) {
        perror("bind");
        exit(1);
    }
    if ( listen(*sockfd, 1024) < 0 ) {
        perror("listen");
        exit(1);
    }
}

int main(int argc, char **argv)
{
    int          listenfd,sockfd, connfd,n;
    int i = 0;
    struct sockaddr_in  servaddr;
    struct sockaddr_in servaddr2;
    time_t        ticks;  
    Appello appelli[maxNumAppelli];   // lista di max 100 esami
    int numEsami = 0;
    
    RichiestaStudente *richiesta;
    RispostaSegreteria *risposta;

    initSegreteria(&listenfd, &servaddr);

    // Inizializzo la lista degli esami disponibili

    inizializzaEsami(appelli, &numEsami);

    for(int i = 0; i < numEsami; i++){
        printf("Esame: %s, data: %s\n", appelli[i].esame, appelli[i].data);
    }

    // mi connetto al server universitario 
    connettiAlServer(&sockfd, &servaddr2);
    // da inviare al server universitario 
    inviaEsamiAlServer(sockfd, appelli, numEsami);
    close(sockfd);

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
        richiesta =(RichiestaStudente *) malloc(sizeof(RichiestaStudente));
        n = FullRead(connfd, &(*richiesta), sizeof(RichiestaStudente));
        printf("\n");
        printf("Richiesta: \n");
        printf("Esame: %s\n", richiesta->esame);
        printf("Azione: %s\n", richiesta->azione);
        printf("\n");
        risposta = (RispostaSegreteria *) malloc(sizeof(RispostaSegreteria));
        risposta->errore = true;
        if((i = cercaEsame(richiesta->esame, appelli, numEsami))> -1){
            // cerco se l'esame richiesto dallo studente è presente nella lista degli esami: 
            strcpy(risposta->azione, richiesta->azione);
            strcpy(risposta->esame, richiesta->esame);
            risposta->errore = false;

            if(strcmp(richiesta->azione, azione1) == 0){
                printf("Vuoi conoscere le date per l'esame %s\n",richiesta->esame);
                memcpy(&risposta->lista.appello, &appelli[i], sizeof(Appello));
            }else if(strcmp(richiesta->azione, azione2) == 0){
                connettiAlServer(&sockfd, &servaddr2);
                printf("Vuoi prenotarti all'appello per l'esame %s\n", richiesta->esame);
                inviaPrenotazione(sockfd, appelli[i], richiesta->matricola);
                memcpy(&risposta->prenotazione.appello, &appelli[i], sizeof(Appello));
                close(sockfd);
            }else{
                printf("Azione non supportata, addio\n");
            }
        }
        FullWrite(connfd, (void*)&(*risposta), sizeof(RispostaSegreteria));
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

