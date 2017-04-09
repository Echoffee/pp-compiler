/* environ.h */
#ifndef ENVIRON_H
#define ENVIRON_H
/* ----------------------------types--------------------------------------------*/
/* environnement := liste de couples (identificateur, entier) */
typedef struct array{
  int* values;
  int size;
}array;

typedef struct cellenv{
  char *ID;
  int  VAL;
  array* AR ;
  struct cellenv *SUIV;} *ENV;

/*------------------FONCTIONS -----------------------------------------------------*/
/*---------------------allocation memoire------------------------------------------*/
extern char *Idalloc();      /* retourne un tableau de MAXIDENT char               */
extern ENV Envalloc();       /* retourne un ENV                                    */
/*---------------------environnements----------------------------------------------*/
extern int initenv(ENV *prho,char *var);/* initialise l'ident var dans *prho       */
extern int ecrire_env(ENV rho);/* affiche l'environnement                          */
extern int eval(int op, int arg1, int arg2); /* retourne (arg1 op arg2)            */
extern ENV rech(char *chaine, ENV listident);/* retourne la position de chaine     */
extern int affect(ENV rho, char *var, int val);/* affecte val a la variable var    */
extern int valch(ENV rho, char *var); /* valeur de var dans envirnt rho            */
extern void removeLastFromEnv(ENV rho); /* supprime le dernier elem de rho         */
/* ------------------CONSTANTES ---------------------------------------------------*/
#define MAXIDENT 16          /* long max d'un identificateur de variable           */
#define MAXQUAD  5*MAXIDENT  /* long max d'un quadruplet c3a                       */

/*-------------------TABLEAUX -----------------------------------------------------*/


extern int ar_read(array* arr,int i); /* retourne arr[i]*/
extern void ar_write(array* arr, int i, int value); /* réalise arr[i] = value */
extern int affect_ar(ENV rho, char *t,int i, int val);
extern ENV rech_ar(char *t, ENV listident);
extern int valch_ar(ENV rho, char *t, int i);
extern int initenv_ar(ENV *prho,char *t,int i,int size);
extern void print_ar(array *arr);
#endif
