/* environ.c */
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "environ.h"
#include "bilquad.h"
/*---------------------allocation memoire----------------------------*/
char *Idalloc()
{
  return((char *)malloc(MAXIDENT*sizeof(char)));
}
ENV Envalloc()
{
  return((ENV)malloc(sizeof(struct cellenv)));
}

/*-------------------------------------------------------------------*/
/*-----------------------------environnements------------------------*/

/* initialise l'environnement *prho par  var=0    */
/* la chaine var est copiee dans l' environnement */
int initenv(ENV *prho,char *var)
{ENV pos, newcell;
  pos=rech(var,*prho);/* adresse de la cellule contenant var */
  if (pos == NULL)
    /*on insere var en tete de envrnt*/
    { newcell=Envalloc();
      newcell->ID=Idalloc();
      strcpy(newcell->ID,var);
      newcell->VAL=0;
      newcell->SUIV=*prho;
      *prho=newcell;
      return (EXIT_SUCCESS);
    }
  else
    {
      return(EXIT_FAILURE);
    }
}
/* retourne (arg1 op arg2) */
int eval(int op, int arg1, int arg2)
{switch(op)
    {case Pl:
	return(arg1 + arg2);
    case Mo:
      return(arg1 - arg2);
    case Mu:
      return(arg1 * arg2);
    case Not:
      return (arg1+1)%2;
    case And:
      return arg1 && arg2;
    case Or:
      return arg1 || arg2;
    case Lt:
      return (arg1 < arg2);
    default:
      return(0);
    }
  return(0);
}

/* retourne l'adresse de la cellule contenant chaine. NULL si la chaine est absente */
ENV rech(char *chaine, ENV listident)
{if (listident!=NULL)
  {
    if (listident->ID && strcmp(listident->ID,chaine)==0)
        {return listident;}
      else{
	     return rech(chaine,listident->SUIV);
     }
    }
  else
    return NULL;
}

/* affecte val a la variable var , dans rho */
int affect(ENV rho, char *var, int val)
{ENV pos;
  pos=rech(var,rho);/* adresse de la cellule contenant var */
  if (pos != NULL)
    {(pos->VAL)=val;
      return(EXIT_SUCCESS);
    }
  else
    return(EXIT_FAILURE);
}

/* affiche l'environnement */
int ecrire_env(ENV rho)
{ if (rho==NULL)
    {printf("fin d' environnement \n");
      return(EXIT_SUCCESS);}
  else
    {
        if(rho->ID != NULL && strncmp(rho->ID,"CT",2) !=0 && strncmp(rho->ID,"ET",2) != 0 && rho->AR == NULL){
           printf("variable %s valeur %d \n",rho->ID ? rho->ID : "(null)",rho->VAL);
        }
        if(rho->ID != NULL && strncmp(rho->ID,"CT",2) !=0 && strncmp(rho->ID,"ET",2) != 0 && rho->AR != NULL){
          printf("tableau %s valeurs ",rho->ID);
          print_ar(rho->AR);
        }
        ecrire_env(rho->SUIV);
        return(EXIT_SUCCESS);
    };
}

/* valeur de var dans rho */
int valch(ENV rho, char *var)
{ENV pos;
  pos=rech(var,rho);/* adresse de la cellule contenant var */
  if (pos != NULL){
    return(pos->VAL);
  }
  else
    return(0);
}

void removeLastFromEnv(ENV rho)
{
    if (rho->SUIV == NULL){
      rho = NULL;
    } else {
      removeLastFromEnv(rho->SUIV);
    }
}

array* new_ar(int size){
  array* a = malloc(sizeof(struct array));
  a->size = size;
  a->values = malloc(size*sizeof(int));
  return a;
}

int ar_read(array* arr, int i)
{
  if(i >= arr->size){
    fprintf(stderr,"Index is out of bounds, value set to 0\n");
    return 0;
  }
  else
    return arr->values[i];
}

void ar_write(array* arr, int i, int val)
{
  if (i >= arr->size)
    fprintf(stderr,"Index is out of bounds\n");
  else
    arr->values[i] = val;
}

int initenv_ar(ENV *prho,char *t,int i,int size)
{ENV pos, newcell;
  pos=rech_ar(t,i,*prho);/* adresse de la cellule contenant var */
  if (pos == NULL)
    /*on insere var en tete de envrnt*/
    { newcell=Envalloc();
      newcell->ID=Idalloc();
      strcpy(newcell->ID,t);
      newcell->AR=new_ar(size);
      newcell->SUIV=*prho;
      *prho=newcell;
      return (EXIT_SUCCESS);
    }
  else
    {
      return(EXIT_FAILURE);
    }
}

int valch_ar(ENV rho, char *t, int i)
{ENV pos;
  pos=rech_ar(t,i,rho);/* adresse de la cellule contenant var */
  if (pos != NULL)
    return(ar_read(pos->AR,i));
  else
    return(0);
}

ENV rech_ar(char *t, int i, ENV listident)
{if (listident!=NULL)
    {if (listident->ID && strcmp(listident->ID,t)==0 && listident->AR)
        {return listident;}
      else
	return rech_ar(t,i,listident->SUIV);
    }
  else
    return NULL;
}

int affect_ar(ENV rho, char *t,int i, int val)
{ENV pos;
  pos=rech_ar(t,i,rho);/* adresse de la cellule contenant var */
  if (pos != NULL)
    {ar_write(pos->AR,i,val);
      return(EXIT_SUCCESS);
    }
  else
    return(EXIT_FAILURE);
}

void print_ar(array* arr)
{
for(int i = 0; i < arr->size; i++)
  printf("%d ",arr->values[i]);
printf("\n");
}
