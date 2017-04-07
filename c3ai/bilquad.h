/* bilquad.h */
#ifndef BILQUAD_H
#define BILQUAD_H
#define Pl 257
#define Mo 258
#define Mu 259
#define Af 260
#define Afc 261
#define Sk 262
#define Jp 263
#define Jz 264
#define St 265
#define Not 266
#define AfInd 267
#define Ind 268
#define Lt 269
#define Or 270
#define And 271
#define Param 272
#define Call 273
#define Ret 274
#define STACK_MAX 200

typedef struct stack{
	void* data[STACK_MAX];
	int size;
}Stack;

/* ----------------------------types--------------------------------------------*/
/* biliste de quadruplets etiquetes (stocke C3A ou Y86 )*/
typedef struct cellquad{
  char *ETIQ;
  int  OP;
  char *ARG1, *ARG2, *RES;
  struct cellquad *SUIV;} *QUAD;

typedef struct{
  QUAD debut;
  QUAD fin;} BILQUAD;

/*---------------------fonctions ------------------------------------------------*/
extern QUAD creer_quad(char *etiq,int op,char *arg1,char *arg2,char *res);
extern BILQUAD bilquad_vide() ;                    /* retourne une biliste vide  */
extern BILQUAD creer_bilquad(QUAD qd); /* retourne une biliste  a un element     */
extern QUAD rechbq(char *chaine, BILQUAD bq);/*ret le quad etiquete par chaine   */
extern BILQUAD concatq(BILQUAD bq1, BILQUAD bq2);/* retourne la concatenation    */
extern char *nomop(int codop); /* traduit entier vers chaine (= nom operation)    */
extern void ecrire_quad(QUAD qd); /* affiche le quadruplet                       */
extern void ecrire_bilquad(BILQUAD bq); /* affiche la biliste de quadruplets     */
extern void *Top(Stack*);
extern void Init(Stack*);
extern void Pop(Stack*);
extern void Push(Stack*,void*);

#endif
