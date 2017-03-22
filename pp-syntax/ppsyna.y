%error-verbose

%{
	#include <stdio.h>
	#include "ppsyna.h"
	
	int yylex();
	int yyerror(char* s);
%}
%union {
	int ival;
	char* sval;
}

%token T_ar NewAr T_boo T_int Def Dep Af Sk V_TRUE V_FALSE Se If Th El Var Wh Do Pl Mo Mu And Or Not Lt Eq
%token S_OP S_CL P_OP P_CL B_OP B_CL S_C S_DP
%token T_int T_str

%type<ival> T_int
%type<sval> T_str

%start MP
%%

MP		: L_vart LD C
		;
	
E		: E Pl E
		| E Mo E
		| E Mu E
		| E Or E
		| E Lt E
		| E Eq E
		| E And E
		| Not E
		| P_OP E P_CL
		| I
		| V
		| V_TRUE
		| V_FALSE
		| V P_OP L_args P_CL
		| NewAr TP B_OP E B_CL
		| Et
		;
	
Et		: V B_OP E B_CL
		| Et B_OP E B_CL
		;
	
C		: C Se C
		| CC 
		| If E Th CC El CC
		| Wh E Do CC
		| V P_OP L_args P_CL
		;

CC		: Et Af E
		| V Af E
		| Sk
		| A_OP C A_CL
		;
	
L_args	: %empty
		| L_argsnn
		;
		
L_argsnn: E
		| L_argsnn S_C L_argsnn
		;
		
L_argt	: %empty
		| L_argtnn
		;
		
L_argtnn: Argt
		| L_argtnn S_C Argt
		;
		
Argt	: V S_DP TP
		;
		
TP		: T_boo
		| T_int
		| T_ar TP
		;
		
L_vart	: %empty
		| L_vartnn
		;
		
L_vartnn: Var Argt
		| L_vartnn S_C Var Argt
		;
		
D_entp	: Dep NPro P_OP L_argt P_CL
		;
		
D_entf	: Def NFon P_OP L_argt P_CL S_DP TP
		;

D		: D_entp L_vart C
		| D_entf L_vart C
		;
		
LD		: %empty
		| LD D
		;

NPro	: T_str
		;
		
NFon	: T_str
		;
		
V		: T_str
		;