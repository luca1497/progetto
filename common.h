#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const char azione1[] = {'L','I','S','T','A','D','A','T','E','\0'};
const char azione2[] = {'P','R','E','N','O','T','A','Z','I','O','N','E','\0'};
const int maxSize = 100;
const int segreteriaPort = 1024;
const int serverPort = 1026;
const int maxNumAppelli = 100;
const char *ip = "127.0.0.1";
typedef char Esame[maxSize];
typedef char Azione[maxSize];
typedef char Data[maxSize];
typedef char Matricola[10];

typedef struct Appello_str{
  Esame esame;
  Data data;
} Appello;

typedef struct RichiestaStudente_str{
  Esame esame;
  Azione azione;
  Matricola matricola;
} RichiestaStudente;

typedef struct RispostaSegreteriaLista_str{
    Appello appello;
} RispostaSegreteriaLista;

typedef struct RispostaSegreteriaPrenotazione_str{
    Appello appello;
} RispostaSegreteriaPrenotazione;

typedef struct RispostaSegreteria_str{
  Esame esame;
  Azione azione;
  bool errore;// = false;
  RispostaSegreteriaLista lista;// = NULL;
  RispostaSegreteriaPrenotazione prenotazione;//= NULL;
} RispostaSegreteria;

typedef struct Prenotazione_str{
  Appello appello;
  Matricola matricola;
} Prenotazione;

typedef struct RichiestaSegreteria_str{
  int type; // 1 per caricamento esami, 2 per caricamento appello studente
  int numAppelli;
  Appello appelli[maxNumAppelli];
  Prenotazione prenotazione;
} RichiestaSegreteria;


typedef struct RispostaServer_str{
  bool errore; 
  int numAppelli; 
}RispostaServer; 


#endif 
