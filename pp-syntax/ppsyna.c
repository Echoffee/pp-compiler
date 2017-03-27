#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppsyna.h"

pp_func f_context = NULL;
pp_func f_root = NULL;
pp_func f_current = NULL;

pp_type syna_create_type(pp_type_id type, pp_type next)
{
	pp_type t = (pp_type) malloc(sizeof(struct s_pp_type));
	t->type = type;
	t->next = next;
	
	return t;
}

pp_var env_add_variable(char* name, pp_type type)
{
	pp_var v = (pp_var) malloc(sizeof(struct s_pp_var));
	v->name = strdup(name);
	v->type = type;
	v->next = NULL;
	
	if (f_context->context_current != NULL)
		f_context->context_current->next = v;
		
	if (f_context->context == NULL)
		f_context->context = v;
		
	f_context->context_current = v;
	
	return v;
}

pp_var lcl_add_variable(pp_func func, char* name, pp_type type)
{
	pp_var v = (pp_var) malloc(sizeof(struct s_pp_var));
	v->name = strdup(name);
	v->type = type;
	v->next = NULL;
	
	if (func->args_current != NULL)
		func->args_current->next = v;
		
	if (func->args == NULL)
		func->args = v;
		
	func->args_current = v;
	
	return v;
}

void env_add_function(char* name, pp_type ret_type, pp_var args)
{
	pp_func f = (pp_func) malloc(sizeof(struct s_pp_func));
	f->name = strdup(name);
	f->ret_type = ret_type;
	f->args = args;
	f->args_current = f->args;
	f->context = NULL;
	f->context_current = NULL;
	f->body = NULL;
	if (f_current != NULL)
		f_current->next = f;
		
	if (f_root == NULL)
		f_root = f;
		
	f_current = f;
}

pp_var env_add_lcl_variable(pp_var lcl_parent, char* name, pp_type type)
{
	pp_var v =  (pp_var) malloc(sizeof(struct s_pp_var));
	v->name = strdup(name);
	v->type = type;
	v->next = NULL;
	
	if (lcl_parent != NULL)
		lcl_parent->next = v;
		
	return v;
}

void env_initialize()
{
	env_add_function("main_program", syna_create_type(NONE, NULL), NULL);
	env_change_context("main_program");
}

void env_change_context(char* context_name)
{
		pp_func f = f_root;
		while (f != NULL && strcmp(f->name, context_name))
			f = f->next;
			
		if (f == NULL)
			fprintf(stderr, "ERROR : Context '%s' not found\n", context_name);
			
		f_context = f;
		fprintf(stderr, "Context changed to %s\n", f_context->name);
}

pp_var env_get_variable(char* name)
{
	pp_func c = f_context;
	pp_var v = c->context;
	while (v != NULL && strcmp(v->name, name))
		v = v->next;
		
	if (v != NULL)
		return v;
		
	fprintf(stderr, "Variable not found in local context...\n");
	env_change_context("main_program");
	v = f_context->context;
	while (v != NULL && strcmp(v->name, name))
		v = v->next;
	
	if (v == NULL)
		fprintf(stderr, "ERROR : Variable '%s' not found (current context : '%s')\n", name, c->name);
		
	fprintf(stderr, "Going back to local context...\n");
	f_context = c;
	v = env_add_variable(name, NONE);
	return v;
}

void display_args(pp_var lcl_root, int rank)
{
	if (lcl_root != NULL)
	{
		if (lcl_root->next != NULL)
			display_args(lcl_root->next, rank + 1);
		
		printf("%s :", lcl_root->name);
		pp_type t_current = lcl_root->type;
		while (t_current != NULL)
		{
			char s[9];
			switch (t_current->type) {
				case NONE:
					sprintf(s, "none");
					break;
				
				case INT:
				 	sprintf(s, "integer");
					break;
					
				case BOOL:
					sprintf(s, "boolean");
					break;
					
				case ARRAY:
					sprintf(s, "array of");
					break;
			}
			
			printf(" %s", s);
			t_current = t_current->next;
		}
		
		if (rank > 0)
			printf(", ");
	}
}

void env_display()
{
	printf("===== ENV =====\n");
	pp_func f = f_root;
	while(f != NULL)
	{
		pp_var v_root = f->context;
		while (v_root != NULL)
		{
			
			printf("Var %s of type", v_root->name);
			pp_type t_current = v_root->type;
			while (t_current != NULL)
			{
				char s[9];
				switch (t_current->type) {
					case NONE:
					sprintf(s, "none");
					break;
					
					case INT:
					sprintf(s, "integer");
					break;
					
					case BOOL:
					sprintf(s, "boolean");
					break;
					
					case ARRAY:
					sprintf(s, "array of");
					break;
				}
				
				printf(" %s", s);
				t_current = t_current->next;
			}
			
			printf(" in context '%s'.\n", f->name);
			v_root = v_root->next;
		}
		
		f = f->next;
	}
	printf("-----\n");
	while (f_root != NULL)
	{
		char s[10];
		switch (f_root->ret_type->type) {
			case NONE:
			sprintf(s, "Procedure");
			break;
			
			default:
			sprintf(s, "Function");
			break;
		}
		
		printf("%s %s (", s, f_root->name);
		display_args(f_root->args, 0);
		printf(")");
		if (f_root->ret_type->type != NONE)
		{
			printf(" :");
			pp_type t_current = f_root->ret_type;
			while (t_current != NULL)
			{
				char s[9];
				switch (t_current->type) {
					case INT:
					sprintf(s, "integer");
					break;
					
					case BOOL:
					sprintf(s, "boolean");
					break;
					
					case ARRAY:
					sprintf(s, "array of");
					break;
				}
				
				printf(" %s", s);
				t_current = t_current->next;
			}
		}
		
		if (f_root->body != NULL)
		{
			printf("\n{\n");
			syna_display(f_root->body);
			printf("\n}\n");
		}else{
			printf(";\n");
		} 
		
		f_root = f_root->next;
	}
	
	printf("===== END =====\n");
}

//AST

syna_node syna_create_node(int num_childs)
{
	syna_node n = (syna_node) malloc(sizeof(struct s_syna_node));
	n->type = NEMPTY;
	n->value = 0;
	n->value_type = NONE;
	n->variable = NULL;
	n->function = NULL;
	n->opi = INONE;
	n->opb = BNONE;
	n->string = NULL;
	n->childs = (syna_node*) malloc(sizeof(struct s_syna_node) * num_childs);
	
	return n;
}

syna_node syna_opi_node(syna_node member_left, syna_node member_right, syna_opi op)
{
	syna_node n = syna_create_node(2);
	n->type = NOPI;
	n->childs[0] = member_left;
	n->childs[1] = member_right;
	n->value_type = syna_create_type(INT, NULL);
	n->opi = op;
	
	return n;
}

syna_node syna_opb_node(syna_node member_left, syna_node member_right, syna_opb op)
{
	syna_node n = syna_create_node(2);
	n->type = NOPB;
	n->childs[0] = member_left;
	n->childs[1] = member_right;
	n->value_type = syna_create_type(BOOL, NULL);
	n->opb = op;
	
	return n;
}

syna_node syna_p_node(syna_node content)
{
	syna_node n = syna_create_node(1);
	n->type = NPBA;
	n->childs[0] = content;
	
	return n;
}

syna_node syna_int_node(int value)
{
	syna_node n = syna_create_node(0);
	n->type = NVALUE;
	n->value = value;
	n->value_type = syna_create_type(INT, NULL);
	
	return n;
}

syna_node syna_var_node(char* name)
{
	syna_node n = syna_create_node(0);
	n->type = NVAR;
	//n->variable = env_get_variable(name);
	n->string = strdup(name);
	n->value_type = n->variable->type;
	
	return n;
}

syna_node syna_bool_node(int value)
{
	syna_node n = syna_create_node(0);
	n->type = NVALUE;
	n->value = value;
	n->value_type = syna_create_type(BOOL, NULL);
	
	return n;
}

syna_node syna_array_node(syna_node member, syna_node index)
{
	//Find a way to get an object from an array 1d+
	syna_node n = syna_create_node(2);
	n->type = NARRAY;
	n->childs[0] = member;
	n->childs[1];
	
	return n;
}

syna_node syna_branch_node(syna_node left, syna_node right)
{
	syna_node n = syna_create_node(2);
	n->type = NBRANCH;
	n->childs[0] = left;
	n->childs[1] = right;
	
	return n;
}

syna_node syna_ITE_node(syna_node cond, syna_node th, syna_node el)
{
	syna_node n = syna_create_node(3);
	n->type = NITE;
	n->childs[0] = cond;
	n->childs[1] = th;
	n->childs[2] = el;
	
	return n;
}

syna_node syna_WD_node(syna_node cond, syna_node d)
{
	syna_node n = syna_create_node(2);
	n->type = NWD;
	n->childs[0] = cond;
	n->childs[1] = d;
	
	return n;
}

syna_node syna_aaf_node(syna_node dest, syna_node value)
{
	syna_node n = syna_create_node(2);
	n->type = NAAF;
	n->childs[0] = dest;
	n->childs[1] = value;
	
	return n;
}

syna_node syna_vaf_node(syna_node dest, syna_node value)
{
	syna_node n = syna_create_node(2);
	n->type = NVAF;
	n->childs[0] = dest;
	n->childs[1] = value;
	
	return n;
}

syna_node syna_skip_node()
{
	syna_node n = syna_create_node(0);
	n->type = NSKIP;
	
	return n;
}

syna_node syna_a_node(syna_node content)
{
	syna_node n = syna_create_node(1);
	n->type = NPBA;
	n->childs[0] = content;
	
	return n;
}

syna_node syna_empty_node()
{
	syna_node n = syna_create_node(0);
	
	return n;
}

syna_node syna_expr_node(syna_node expr)
{
	syna_node n = syna_create_node(1);
	n->type = NEXPR;
	n->childs[0] = expr;
	
	return n;
}

syna_node syna_vdef_node(syna_node dest, syna_node type)
{
	syna_node n = syna_create_node(2);
	n->type = NVDEF;
	n->childs[0] = dest;
	n->childs[1] = type;
	n->value_type = type->value_type;
	
	return n;
}

syna_node syna_type_node(pp_type_id type, syna_node next)
{
	syna_node n = syna_create_node(1);
	n->type = NTYPE;
	n->childs[0] = next;
	n->value_type = syna_create_type(type, NULL); //incorrect (1d only)

	return n;
}

syna_node syna_pdef_node(char* name, syna_node args)
{
	syna_node n = syna_create_node(1);
	n->type = NPDEF;
	n->string = strdup(name);
	n->childs[0] = args;
	
	return n;
}

syna_node syna_fdef_node(char* name, syna_node args, syna_node ret)
{
	syna_node n = syna_create_node(2);
	n->type = NFDEF;
	n->string = strdup(name);
	n->childs[0] = args;
	n->childs[1] = ret;
	
	return n;
}

syna_node syna_root_node(syna_node vart, syna_node ld, syna_node c)
{
	syna_node n = syna_create_node(3);
	n->type = NROOT;
	n->childs[0] = vart;
	n->childs[1] = ld;
	n->childs[2] = c;
	
	return n;
}

syna_node syna_new_var_node(char* name)
{
	syna_node n = syna_create_node(0);
	n->type = NNVAR;
	n->string = strdup(name);
	//n->variable = env_add_variable(name, syna_create_type(NONE, NULL));
	
	return n;
}

syna_node syna_pbody_node(syna_node def, syna_node def_vars, syna_node body)
{
	syna_node n = syna_create_node(3);
	n->type = NPBODY;
	n->childs[0] = def;
	n->childs[1] = def_vars;
	n->childs[2] = body;
	
	return n;
}

syna_node syna_fbody_node(syna_node def, syna_node def_vars, syna_node body)
{
	syna_node n = syna_create_node(3);
	n->type = NFBODY;
	n->childs[0] = def;
	n->childs[1] = def_vars;
	n->childs[2] = body;
	
	return n;
}

syna_node syna_call_func_node(char* name, syna_node args)
{
	return NULL;
}

syna_node syna_newarray_node(syna_node type, syna_node expr)
{
	return NULL;
}

void syna_link_args_to_func(pp_func func, syna_node args)
{
	switch (args->type) {
		case NVDEF:
			syna_execute(args->childs[0]);
			syna_execute(args->childs[1]);
			lcl_add_variable(func, args->childs[0]->string, args->childs[1]->value_type);
		break;
	}
}


void syna_execute(syna_node root)
{
	switch (root->type) {
		case NEMPTY:
			//eh
			break;
		
		case NROOT:
			syna_execute(root->childs[0]);
			syna_execute(root->childs[1]);
			syna_execute(root->childs[2]);
			break;
		
		case NOPI:
			syna_execute(root->childs[0]);
			syna_execute(root->childs[1]);
			switch (root->opi) {
				case INONE:
					//eh
					break;
				
				case PL:
					//root->value = root->childs[0]->value + root->childs[1]->value;
					break;
					
				case MO:
					//root->value = root->childs[0]->value - root->childs[1]->value;
					break;
					
				case MU:
					//root->value = root->childs[0]->value * root->childs[1]->value;
					break;
			}
			break;
		
		case NOPB:
			syna_execute(root->childs[0]);
			syna_execute(root->childs[1]);
			switch (root->opi) {
				case INONE:
					//eh
					break;
				
				case OR:
					//root->value = (root->childs[1]->value + root->childs[0]->value > 0? 1 : 0);
					break;
				
				case LT:
					//root->value = (root->childs[1]->value < root->childs[0]->value? 1 : 0);
					break;
				
				case EQ:
					//root->value = (root->childs[1]->value == root->childs[0]->value? 1 : 0);
					break;
				
				case AND:
					//root->value = (root->childs[1]->value * root->childs[0]->value > 0? 1 : 0);
					break;
				
				case NOT:
					//root->value = 0;
					break;
			}
			break;
		
		case NPBA:
		   syna_execute(root->childs[0]);
		break;
		
		case NVALUE:
			//end
		break;
		
		case NVAR:
			
		break;
		
		case NARRAY:
			  
		break;
		
		case NBRANCH:
			syna_execute(root->childs[0]);
			syna_execute(root->childs[1]);
		break;
		
		case NITE:
				
		break;
		
		case NWD:
				 
		break;
		
		case NAAF:
				  
		break;
		
		case NNVAR:

		break;
		
		case NVAF:
			syna_execute(root->childs[1]);
			syna_execute(root->childs[0]);
			root->childs[0]->variable->type = root->childs[1]->value_type;
			break;
		
		case NSKIP:
			//eh	
			break;
		
		case NEXPR:
			
		break;
		
		case NVDEF:
			root->childs[0]->variable = env_get_variable(root->childs[0]->string);
			if (root->childs[1] != NULL)
				syna_execute(root->childs[1]);
				
			root->childs[0]->variable->type = root->value_type;
			//env_add_variable(root->childs[0]->variable->name, root->value_type);
			break;
		
		case NTYPE: 
			if (root->childs[0] != NULL)
			{
				syna_execute(root->childs[0]);
				root->value_type->next = root->childs[0]->value_type;
			}
		break;
		
		case NPDEF:
			env_add_function(root->string, syna_create_type(NONE, NULL), root->variable);
			env_change_context(root->string);
			syna_link_args_to_func(f_context, root->childs[0]);	//define args						
		break;
		
		case NFDEF:
			syna_execute(root->childs[1]); //define ret type
			root->value_type = root->childs[1]->value_type;
			env_add_function(root->string, root->value_type, root->variable);
			env_change_context(root->string);
			syna_link_args_to_func(f_context, root->childs[0]);	//define args
		break;
	
		case NPBODY:
		syna_execute(root->childs[0]); //Procedure declaration
		//Context already changed
		syna_execute(root->childs[1]); //Add local variables
		//When called, must execute root->childs[2]
		f_context->body = root->childs[2];
		env_change_context("main_program");
		break;
		
		case NFBODY:
		syna_execute(root->childs[0]); //Function declaration
		//Context already changed
		syna_execute(root->childs[1]); //Add local variables
		//When called, must execute root->childs[2]
		f_context->body = root->childs[2];
		env_change_context("main_program");
		break;
	}
}

void syna_display(syna_node root)
{
	switch (root->type) {
		case NEMPTY:
			//eh
			break;
		
		case NOPI:
			syna_display(root->childs[0]);
			switch (root->opi) {
				case INONE:
					//eh
					break;
				
				case PL:
					printf(" +");
					break;
					
				case MO:
					printf(" -");
					break;
					
				case MU:
					printf(" *");
					break;
			}
			
			syna_display(root->childs[1]);
			break;
		
		case NOPB:
			syna_display(root->childs[0]);
			switch (root->opi) {
				case INONE:
					//eh
					break;
				
				case OR:
					printf(" OR");
					break;
				
				case LT:
					printf(" <=");
					break;
				
				case EQ:
					printf(" ==");
					break;
				
				case AND:
					printf(" AND");
					break;
				
				case NOT:
					printf(" ~");
					break;
			}
			
			syna_display(root->childs[1]);
			break;
		
		case NPBA:
		   syna_execute(root->childs[0]);
		break;
		
		case NVALUE:
			if (root->value_type->type == BOOL)
				if (root->value == 1)
					printf(" True");
				else
					printf(" False");
			else
				printf(" %d", root->value);
		break;
		
		case NVAR:
			printf(" %s", root->string);
		break;
		
		case NARRAY:
			  
		break;
		
		case NBRANCH:
			syna_display(root->childs[0]);
			syna_display(root->childs[1]);
		break;
		
		case NITE:
			printf(" If (");
			syna_display(root->childs[0]);
			printf(" Th {");
			syna_display(root->childs[1]);
			printf("} El {");
			syna_display(root->childs[2]);
			printf("}");
		break;
		
		case NWD:
			printf(" Wh (");
			syna_display(root->childs[0]);
			printf(") Do {");
			syna_display(root->childs[1]);
			printf("}");
		break;
		
		case NAAF:
			syna_display(root->childs[0]);
			syna_display(root->childs[1]);
		break;
		
		case NVAF:
			syna_display(root->childs[0]);
			printf(" Af");
			syna_display(root->childs[1]);
			break;
		
		case NSKIP:
			printf(" Sk");
			break;
		
		case NEXPR:
			syna_display(root->childs[0]);
		break;
		
	}
}