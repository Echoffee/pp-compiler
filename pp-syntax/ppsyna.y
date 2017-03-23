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
	
E		: E Pl E { $$ = syna_opi_node($1, $3, PL); } 
		| E Mo E { $$ = syna_opi_node($1, $3, MO); }
		| E Mu E { $$ = syna_opi_node($1, $3, MU); }
		| E Or E { $$ = syna_opb_node($1, $3, OR); }
		| E Lt E { $$ = syna_opb_node($1, $3, LT); }
		| E Eq E { $$ = syna_opb_node($1, $3, EQ); }
		| E And E { $$ = syna_opb_node($1, $3, AND); }
		| Not E { $$ = syna_opb_node($2, NULL, NOT); }
		| P_OP E P_CL { $$ = syna_p_node($2); }
		| I { $$ = syna_int_node($1); }
		| V { $$ = syna_var_node($1); }
		| V_TRUE { $$ = syna_bool_node(1); }
		| V_FALSE { $$ = syna_bool_node(0); }
		| V P_OP L_args P_CL //?????????????????????????????????????????
		| NewAr TP B_OP E B_CL //???????????????????????????????????????
		| Et { $$ = $1; }
		;
	
Et		: V B_OP E B_CL { $$ = syna_array_node(syna_var_node($1), $3); }
		| Et B_OP E B_CL { $$ = syna_array_node($1, $3); }
		;
	
C		: C Se C { $$ = syna_branch_node($1, $3); }
		| CC { $$ = $1; }
		| If E Th CC El CC { $$ = syna_ITE_node($2, $4, $6); }
		| Wh E Do CC { $$ = syna_WD_node($2, $4); }
		| V P_OP L_args P_CL //???????????????????????????????????????????
		;

CC		: Et Af E { $$ = syna_aaf_node($1, $3); }
		| V Af E { $$ = syna_vaf_node($1, $3); }
		| Sk { $$ = syna_skip_node(); }
		| A_OP C A_CL { $$ = syna_a_node($2); }
		;
	
L_args	: %empty { $$ = syna_empty_node(); }
		| L_argsnn { $$ = $1; }
		;
		
L_argsnn: E { $$ = syna_expr_node($1); }
		| L_argsnn S_C L_argsnn { $$ = syna_branch_node($1, $3); }
		;
		
L_argt	: %empty { $$ = syna_empty_node(); }
		| L_argtnn { $$ = $1; }
		;
		
L_argtnn: Argt  { $$ = $1; }
		| L_argtnn S_C Argt { $$ = syna_branch_node($1, $3); }
		;
		
Argt	: V S_DP TP { $$ = syna_adef_node($1, $3);} //array definition
		;
		
TP		: T_boo { $$ = syna_type_node(BOOL, NULL); }
		| T_int { $$ = syna_type_node(INT, NULL); }
		| T_ar TP { $$ = syna_type_node(ARRAY, $2); }
		;
		
L_vart	: %empty { $$ = syna_empty_node(); }
		| L_vartnn { $$ = $1; }
		;
		
L_vartnn: Var Argt //??????????????????????????????????????????
		| L_vartnn S_C Var Argt //??????????????????????????????????????????
		;
		
D_entp	: Dep NPro P_OP L_argt P_CL { $$ = syna_pdef_node($1, $3); }
		;
		
D_entf	: Def NFon P_OP L_argt P_CL S_DP TP { $$ = syna_fdef_node($1, $3, $6); }
		;

D		: D_entp L_vart C	//Doesn't work for loops
		| D_entf L_vart C	//also ?????????????????????????
		;
		
LD		: %empty { $$ = syna_empty_node(); }
		| LD D { $$ = syna_branch_node($1, $2); }
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