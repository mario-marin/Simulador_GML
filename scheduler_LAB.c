#include <stdio.h>
#include <stdlib.h>

typedef struct mnodo{
  int tipo;
  int index;
  float tiempo;
  struct mnodo *next;
}Evento;

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

Evento *crearEvento(int t, int i,float tt){
  Evento *p=(struct mnodo*)malloc(sizeof(Evento));
  p->tipo=t;
  p->index=i;
  p->tiempo=tt;
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
    printf("(%i,%f) ",p->tipo,p->tiempo);
    p=p->next;
  }
  printf("\n");
  return;
}

int schedulerIsEmpty(){
  return pScheduler==NULL;
}