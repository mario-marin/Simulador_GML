#include <stdio.h>
#include <stdlib.h>
typedef struct mnodo{
  int tipo;
  int usuario;
  int lambda;
  double tiempo;
  struct mnodo *next;
}Evento;

void pushEvento(Evento* pEvento);
Evento *crearEvento(int tipo,int usuario,float tiempo);
Evento *popEvento();

void imprimirScheduler();
int schedulerIsEmpty();


static Evento *pScheduler=NULL;

void pushEvento(Evento* pEvento){
  if(pScheduler==NULL){
    pScheduler=pEvento;
    return;
  }
  Evento *p=pScheduler;
  if(p->tiempo>pEvento->tiempo){
    pEvento->next=p;
    pScheduler= pEvento;
    return;
  }
  while(p->next!=NULL){
    if(p->next->tiempo>pEvento->tiempo){
      pEvento->next=p->next;
      p->next=pEvento;
      return;
    }
    p=p->next;
  } 
  p->next=pEvento;
  return;
}

Evento *crearEvento(int tipo,int usuario,int lambda,float tiempo){
  Evento *p=(struct mnodo *)malloc(sizeof(Evento));
  p->tipo=tipo;
  p->usuario=usuario;
  p->lambda=lambda;
  p->tiempo=tiempo;
  p->next=NULL;
  return p;
}

Evento *popEvento(){
  Evento *p=pScheduler;
  pScheduler=pScheduler->next;
  p->next=NULL;
  return p;
}

void imprimirScheduler(){
  Evento *p=pScheduler;
  while(p!=NULL){
    printf("(%i,%i,%f) ",p->tipo,p->usuario,p->tiempo);
    p=p->next;
  }
  printf("\n");
  return;
}

int schedulerIsEmpty(){
  return pScheduler==NULL;
}
