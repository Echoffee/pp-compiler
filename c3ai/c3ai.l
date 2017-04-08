%{
#define _XOPEN_SOURCE 700
#include "bilquad.h"
#include "environ.h"
#include <string.h>
BILQUAD list;
char *etiq=NULL,*arg1=NULL,*arg2=NULL,*res=NULL;
int operation = 0;
int first = 1;
char *argcpy(char *src){
    int len = strlen(src);
    char *dst = malloc(sizeof(int)*(len+1));
    return strcpy(dst,src);
}
%}
%option nounput noinput
%s OPE ARG_1 ARG_2 DEST
%%

[A-Za-z]+ { //fun
  BEGIN OPE; free(etiq); etiq=argcpy(yytext);
}
(JMP|ET)[0-9]+                {
                            BEGIN OPE; free(etiq); etiq=argcpy(yytext);
                        }
<OPE>:[A-Za-z]+         {
                            BEGIN ARG_1;
                            if(strncmp("Pl",yytext+1,2)==0) {
                                operation = Pl;
                            } else if (strncmp("Mo",yytext+1,2)==0) {
                                operation = Mo;
                            } else if (strncmp("Mu",yytext+1,2)==0) {
                                operation = Mu;
                            } else if (strncmp("Afc",yytext+1,3)==0) {
                                operation = Afc;
                            } else if (strncmp("Af",yytext+1,2)==0) {
                                operation = Af;
                            } else if (strncmp("Sk",yytext+1,2)==0) {
                                operation = Sk;
                            } else if (strncmp("Jp",yytext+1,2)==0) {
                                operation = Jp;
                            } else if (strncmp("Jz",yytext+1,2)==0) {
                                operation = Jz;
                            } else if (strncmp("St",yytext+1,2)==0) {
                                operation = St;
                            } else if (strncmp("And",yytext+1,3)==0){
                                operation = And;
                            } else if (strncmp("Or",yytext+1,2)==0){
                                operation = Or;
                            } else if (strncmp("Lt",yytext+1,2)==0){
                                operation = Lt;
                            } else if (strncmp("Ind",yytext+1,3)==0){
                                operation = Ind;
                            } else if (strncmp("Not",yytext+1,3)==0){
                                operation = Not;
                            } else if (strncmp("AfInd",yytext+1,5)==0){
                                operation = AfInd;
                            } else if (strncmp("Param",yytext+1,5)==0){
                                operation = Param;
                            } else if (strncmp("Call",yytext+1,4)==0){
                                operation = Call;
                            } else if (strncmp("Ret",yytext+1,3)==0){
                                operation = Ret;
                            }
                          }
<ARG_1>:[A-Za-z0-9]*      {
                            BEGIN ARG_2;
                            free(arg1);
                            arg1=argcpy(yytext+1);
                        }
<ARG_2>:[A-Za-z0-9]*        {
                            BEGIN DEST;
                            free(arg2);
                            arg2=argcpy(yytext+1);
                        }
<DEST>:[A-Za-z0-9]*        {
                            BEGIN INITIAL;
                            char *dst=NULL;
                            dst = argcpy(yytext+1);
                            if(first) {
                                first = 0;
                                list = creer_bilquad(creer_quad(etiq,operation,arg1,arg2,dst));
                            } else {
                                BILQUAD tmp = creer_bilquad(creer_quad(etiq,operation,arg1,arg2,dst));
                                list = concatq(list,tmp);

                            }
                            free(dst);
                        }
[ \t\n]                 ;
%%
int main(int argc, char *argv[]) {
    yylex();
    ENV env = Envalloc();
    ENV param = Envalloc();
    ENV loc;
    char* current = NULL;

    QUAD elem;
    elem = list.debut;
    Stack S;
    Init(&S);
    while (elem != NULL) {
        /***Afc***/
        if (elem->OP == Afc) {
            initenv(&env, elem->RES);
            affect(env, elem->RES, atoi(elem->ARG1));
            elem = elem->SUIV;
        }
        /***Pl Mu Mo***/
        else if (elem->OP == Pl || elem->OP == Mu || elem->OP == Mo) {
            initenv(&env, elem->RES);
            initenv(&env, elem->ARG1);
            initenv(&env, elem->ARG2);
            int res = eval(elem->OP, valch(env, elem->ARG1), valch(env, elem->ARG2));
            affect(env, elem->RES, res);
            elem = elem->SUIV;
        }
        /***Af***/
        else if (elem->OP == Af) {
            initenv(&env, elem->ARG1);
            initenv(&env, elem->ARG2);
            affect(env, elem->ARG1, valch(env, elem->ARG2));
            elem = elem->SUIV;
        }
        /***Jp***/
        else if (elem->OP == Jp) {
            QUAD listStart = list.debut;
            char *dest = elem->RES;
            while (strcmp(listStart->ETIQ,dest) != 0)
                listStart = listStart->SUIV;
            elem=listStart;
        }
        /***Je***/
        else if (elem->OP == Jz) {
            QUAD listStart = list.debut;
            char *dest = elem->RES;
            if(valch(env, elem->ARG1) == 0){
                while (strcmp(listStart->ETIQ,dest) != 0){
                    listStart = listStart->SUIV;
                }
                elem=listStart;
            }else{
                elem = elem->SUIV;
            }
        } else if (elem->OP == St){
            break;
        } else if (elem->OP == Sk){
            elem = elem->SUIV;
        } else if (elem->OP == Call){
          QUAD listStart = list.debut;
          char *dest = elem->RES;
          current = dest;
          loc = Envalloc();
          initenv(&loc,elem->ARG1);
          Push(&S,elem->SUIV);
          while (strcmp(listStart->ETIQ,dest) != 0){
              listStart = listStart->SUIV;
            }
          elem = listStart;//Stack WIP
        } else if (elem->OP == Ret){
          elem->SUIV = (QUAD)Top(&S);//Stack
          Pop(&S);
          printf("***environnement local %s --- valeurs\n", current);
          ecrire_env(loc);
          free(loc);
          free(param);
          param = Envalloc();
          elem = elem->SUIV;
        } else if (elem->OP == And){
          initenv(&env, elem->RES);
          initenv(&env, elem->ARG1);
          initenv(&env, elem->ARG2);
          int res = eval(elem->OP, valch(env, elem->ARG1), valch(env, elem->ARG2));
          affect(env, elem->RES, res);
          elem = elem->SUIV;
        } else if (elem->OP == Or){
          initenv(&env, elem->RES);
          initenv(&env, elem->ARG1);
          initenv(&env, elem->ARG2);
          int res = eval(elem->OP, valch(env, elem->ARG1), valch(env, elem->ARG2));
          affect(env, elem->RES, res);
          elem = elem->SUIV;
        } else if (elem->OP == Not){
          initenv(&env,elem->RES);
          initenv(&env,elem->ARG1);
          int res = eval(elem->OP,valch(env,elem->ARG1),valch(env,elem->ARG1));
          affect(env,elem->RES,res);
          elem = elem->SUIV;
        } else if (elem->OP == AfInd){
          ;//needs to handle arrays
        } else if (elem->OP == Ind){
          ;//needs to handle arrays
        } else if (elem->OP == Lt){
          initenv(&env, elem->RES);
          initenv(&env, elem->ARG1);
          initenv(&env, elem->ARG2);
          int res = eval(elem->OP, valch(env, elem->ARG1), valch(env, elem->ARG2));
          affect(env, elem->RES, res);
          elem = elem->SUIV;
        } else if (elem->OP == Param){
          initenv(&param,elem->ARG1);
          initenv(&param,elem->ARG2);
          affect(param,elem->ARG1,valch(env,elem->ARG2));//Ajoute à l'environnement Paramétrique le couple Arg1:=Arg2
        }
    }

    ecrire_env(env);


}
