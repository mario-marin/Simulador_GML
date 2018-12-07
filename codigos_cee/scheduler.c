
#include <stdlib.h>
#include <stdio.h>

typedef struct mnodo{
  int tipo;
  float tiempo;
  int id;
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

Evento *crearEvento(int tipo,float tiempo, int idx){
  Evento *p=(struct mnodo*)malloc(sizeof(Evento));
  p->tipo=tipo;
  p->id=idx;
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
    printf("(%i,%i,%f) \n",p->id,p->tipo,p->tiempo);
    p=p->next;
  }
  return;
}

int schedulerIsEmpty(){
    Evento *p=pScheduler;
    return (p==NULL);
}

int buscarmaxid(){
    int aux=-1;
    Evento *p=pScheduler;
    while(p!=NULL){
        if(p->id > aux){
            aux=p->id;
        }      
            
        p=p->next;
    }
  return aux;
}
