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



((JMP|ET)[0-9]+)|ET[A-Za-z0-9]{1,16}          {
                            BEGIN OPE; free(etiq); etiq=argcpy(yytext);
                        }
<OPE>:[A-Za-z]+         {
                            BEGIN ARG_1;
                            if(strncmp("Pl",yytext+1,2)==0){
                                operation = Pl;
                            } else if (strncmp("Mo",yytext+1,2)==0){
                                operation = Mo;
                            } else if (strncmp("Mu",yytext+1,2)==0){
                                operation = Mu;
                            } else if (strncmp("AfInd",yytext+1,5)==0){
                                operation = AfInd;
                            } else if (strncmp("Afc",yytext+1,3)==0){
                                operation = Afc;
                            } else if (strncmp("Af",yytext+1,2)==0){
                                operation = Af;
                            } else if (strncmp("Sk",yytext+1,2)==0){
                                operation = Sk;
                            } else if (strncmp("Jp",yytext+1,2)==0){
                                operation = Jp;
                            } else if (strncmp("Jz",yytext+1,2)==0){
                                operation = Jz;
                            } else if (strncmp("St",yytext+1,2)==0){
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
    ENV loc = Envalloc();
    ENV tloc = Envalloc();
    char* current = NULL;
    int scope = 0;
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
        else if (elem->OP == Pl || elem->OP == Mu || elem->OP == Mo || elem->OP == Or || elem->OP == And || elem->OP == Lt) {
              ENV re = rech(elem->RES,loc);
              ENV a1 = rech(elem->ARG1,loc);
              ENV a2 = rech(elem->ARG2,loc);
              initenv((re)?&loc:&env,elem->RES);
              initenv((a1)?&loc:&env, elem->ARG1);
              initenv((a2)?&loc:&env, elem->ARG2);
              int res = eval(elem->OP, valch((scope > 0 && rech(elem->ARG1,loc)!= NULL)?loc:env, elem->ARG1) , valch((scope > 0 && rech(elem->ARG2,loc) != NULL)?loc:env, elem->ARG2));
              affect((scope > 0 && rech(elem->RES,loc) != NULL)?loc:env, elem->RES, res);
              elem = elem->SUIV;
        }
        /***Af***/
        else if (elem->OP == Af) {
          if (scope == 0) {
              initenv(&env, elem->ARG1);
              initenv(&env, elem->ARG2);
            }else{
              initenv(&loc, elem->ARG1);
              initenv(&loc, elem->ARG2);
            }
            affect((scope > 0 && rech(elem->ARG1,loc) != NULL)?loc:env, elem->ARG1, valch((scope > 0 && rech(elem->ARG2,loc) != NULL)?loc:env, elem->ARG2));
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
            ENV a1 = rech(elem->ARG1,loc);
            if (!a1){
              if(valch(env, elem->ARG1) == 0){
                  while (strcmp(listStart->ETIQ,dest) != 0){
                      listStart = listStart->SUIV;
                  }
                  elem=listStart;
              }else{
                  elem = elem->SUIV;
              }
            }else{
              if(valch(loc, elem->ARG1) == 0){
                  while (strcmp(listStart->ETIQ,dest) != 0){
                      listStart = listStart->SUIV;
                  }
                  elem=listStart;
              }else{
                  elem = elem->SUIV;
              }
            }
        } else if (elem->OP == St){
            break;
        } else if (elem->OP == Sk){
            elem = elem->SUIV;
        } else if (elem->OP == Call){
          Push(&S,loc);
          tloc = loc;
          scope++;
          // ca1
          QUAD listStart = list.debut;
          char *dest = elem->ARG1;
          current = dest;
          loc = param;
          //ca2 Retrait des paramètres de l'environnement paramétrique
          int nbArg = atoi(elem->ARG2);
          Push(&S,param);
          for(int i = 0; i < nbArg; i++)
            removeLastFromEnv(param);
          //ca3
          initenv(&loc, &(elem->ARG1[2]));
          initenv((scope-1>0)?&(tloc):&env, &(elem->ARG1[2]));
          Push(&S,elem->SUIV);
          param = NULL;
          //ca4
          while (strcmp(listStart->ETIQ,dest) != 0){
              listStart = listStart->SUIV;
            }
          elem = listStart;
        } else if (elem->OP == Ret){
          scope--;
          elem->SUIV = (QUAD)Top(&S);//Récupération de elem->SUIV dans la pile
          Pop(&S);
          param = (ENV)Top(&S); // Récupération de l'environnement paramétrique du précédent Call
          Pop(&S);
          printf("***appel %d à %.16s --- variables locales\n", scope+1,&current[2]);
          ecrire_env(loc);
          ENV retval = rech(&current[2],loc);
          if (scope == 0){
            loc = Envalloc();
            param = Envalloc();
            Pop(&S);
          }
          else{
            loc = (ENV)Top(&S);
            Pop(&S);
          }
          initenv((scope>0)?&loc:&env,&current[2]);
          affect((scope>0)?loc:env,&current[2],retval->VAL);
          elem = elem->SUIV;
        } else if (elem->OP == Not){
          if (scope == 0){
            initenv(&env,elem->RES);
            initenv(&env, elem->ARG1);
          }else{
            initenv(&loc,elem->RES);
            initenv(&loc, elem->ARG1);
          }
          int res = eval(elem->OP,valch((scope > 0 && rech(elem->ARG1,loc)!=NULL)?loc:env,elem->ARG1),valch((scope > 0 && rech(elem->ARG1,loc)!=NULL)?loc:env,elem->ARG1));
          affect((scope > 0 && rech(elem->RES,loc))?loc:env,elem->RES,res);
          elem = elem->SUIV;
        } else if (elem->OP == AfInd){
          char cons[18];
          strcpy(cons,"CT");
          strcat(cons,elem->ARG1);
          initenv((scope>0 && rech(elem->RES,loc) != NULL)?&loc:&env,elem->RES);
          initenv_ar((scope>0)?&loc:&env,elem->ARG1,valch((scope > 0 && rech(elem->ARG2,loc) !=NULL)?loc:env,elem->ARG2),valch((scope > 0 && rech(cons,loc) != NULL)?loc:env,cons));
          affect_ar((scope > 0 && rech(elem->ARG1,loc) != NULL)?loc:env,elem->ARG1,valch((scope > 0 && rech(elem->ARG2,loc) != NULL)?loc:env,elem->ARG2),valch((scope > 0 && rech(elem->RES,loc) != NULL)?loc:env,elem->RES));
          //needs to handle arrays
          elem=elem->SUIV;
        } else if (elem->OP == Ind){
          char cons[18];
          strcpy(cons,"CT");
          strcat(cons,elem->ARG1);
          initenv((scope>0 && rech(elem->RES,loc))?&loc:&env,elem->RES);
          initenv_ar((scope>0 && rech(elem->ARG1,loc))?&loc:&env,elem->ARG1,valch((scope>0 && rech(elem->ARG2,loc))?loc:env,elem->ARG2),valch((scope > 0 && rech(cons,loc))?loc:env,cons));
          affect((scope > 0 && rech(elem->RES,loc))?loc:env,elem->RES,valch_ar((scope > 0 && rech(elem->ARG1,loc))?loc:env,elem->ARG1,valch((scope > 0 && rech(elem->ARG2,loc))?loc:env,elem->ARG2)));
          //needs to handle arrays
          elem = elem->SUIV;
        } else if (elem->OP == Param){
          initenv(&param,elem->ARG1);
          affect(param,elem->ARG1,valch((scope > 0 && rech(elem->ARG2,loc))?loc:env,elem->ARG2));//Ajoute à l'environnement Paramétrique le couple Arg1:=Arg2
          elem = elem->SUIV;
        }
    }

    ecrire_env(env);


}
