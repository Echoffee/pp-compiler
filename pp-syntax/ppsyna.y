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
	struct e_pp_type type_member;
}

%token T_ar NewAr T_boo T_int Def Dep Af Sk V_TRUE V_FALSE Se If Th El Var Wh Do Pl Mo Mu And Or Not Lt Eq
%token S_OP S_CL P_OP P_CL B_OP B_CL S_C S_DP
%token T_int T_str

%type<ival> T_int
%type<sval> NPro NFon V T_str
%type<type_member> E Et;

%start MP
%%

MP		: L_vart LD C
		;
	
E		: E Pl E { $$ = syna_create_type(INT, NULL); } 
		| E Mo E { $$ = syna_create_type(INT, NULL); }
		| E Mu E { $$ = syna_create_type(INT, NULL); }
		| E Or E { $$ = syna_create_type(BOOL, NULL); }
		| E Lt E { $$ = syna_create_type(BOOL, NULL); }
		| E Eq E { $$ = syna_create_type(BOOL, NULL); }
		| E And E { $$ = syna_create_type(BOOL, NULL); }
		| Not E { $$ = syna_create_type(BOOL, NULL); }
		| P_OP E P_CL { $$ = $2; }
		| I { $$ = syna_create_type(INT, NULL); }
		| V { $$ = env_get_type_of_variable($1) ; }
		| V_TRUE { $$ = syna_create_type(BOOL, NULL); }
		| V_FALSE { $$ = syna_create_type(BOOL, NULL); }
		| V P_OP L_args P_CL //?????????????????????????????????????????
		| NewAr TP B_OP E B_CL {$$ = syna_create_type(ARRAY, $1); }
		| Et { $$ = $1; }
		;
	
Et		: V B_OP E B_CL { $$ = env_get_type_of_variable($1); }
		| Et B_OP E B_CL { $$ = $1; }
		;
	
C		: C Se C
		| CC 
		| If E Th CC El CC
		| Wh E Do CC
		| V P_OP L_args P_CL //???????????????????????????????????????????
		;

CC		: Et Af E
		| V Af E { env_add_variable($1, $3); }
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
		
TP		: T_boo { $$ = syna_create_type(BOOL, NULL); }
		| T_int { $$ = syna_create_type(INT, NULL); }
		| T_ar TP { $$ = syna_create_type(ARRAY, $2); }
		;
		
L_vart	: %empty
		| L_vartnn
		;
		
L_vartnn: Var Argt
		| L_vartnn S_C Var Argt
		;
		
D_entp	: Dep NPro P_OP L_argt P_CL
		;
		
D_entf	: Def NFon P_OP L_argt P_CL S_DP TP { env_add_function($2, $7, $4); env_change_context($2); }
		;

D		: D_entp L_vart C	//Doesn't work for loops
		| D_entf L_vart C
		;
		
LD		: %empty
		| LD D
		;

NPro	: T_str { $$ = $1; }
		;
		
NFon	: T_str { $$ = $1; }
		;
		
V		: T_str { $$ = $1; }
		;
		
%%

int yyerror(char* s)
{
	fprintf(stderr, "*** ERROR: %s***\n", s);
	return 0;
}

int main()
{
	env_initialize();
	yyparse();
	env_display();
	
	return 0;
}