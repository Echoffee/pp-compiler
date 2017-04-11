#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppc3ac.h"

#define ERR_BUFFER_SIZE 256
#define C3A_ETQ_LENGTH 32

pp_func f_context = NULL;
pp_func f_root = NULL;
pp_func f_current = NULL;

int in_fuction_call = 0;
int var_declaration = 0;
int total_errors = 0;
int line_position = 1;

pp_var current_arg = NULL;

pp_context main_context = NULL;

c3a_out c3a_root = NULL;
c3a_out c3a_f = NULL;
c3a_out ccurrent = NULL;
c3a_out cfcurrent = NULL;
int c3a_global_line = 0;
int c3a_global_var = 0;
int c3a_global_arr = 0;
int fun = 0;

int is_af = 0;
int dim = 0;
int lcl = 0;

void c3a_add_line(char* etq, char* op, char* arg1, char* arg2, char* dst)
{
	c3a_out c = (c3a_out) malloc(sizeof(struct s_c3a_out));
	
	char* s = (char*) malloc(sizeof(char) * C3A_ETQ_LENGTH);
	if (etq != NULL)
	sprintf(s, "%s", etq);
	else
	{
		sprintf(s, "ET%d", c3a_global_line);
		c3a_global_line++;
	}
	
	c->etq = s;
	
	char* g = strdup("");
	if (op != NULL)
	c->op = strdup(op);
	else
	c->op = g;
	
	if (arg1 != NULL)
	c->arg1 = strdup(arg1);
	else
	c->arg1 = g;
	
	if (arg2 != NULL)
	c->arg2 = strdup(arg2);
	else
	c->arg2 = g;
	
	if (dst != NULL)
	c->dst = strdup(dst);
	else
	c->dst = g;
	
	if (fun)
	{
		if (cfcurrent != NULL)
		cfcurrent->next = c;
		
		if (c3a_f == NULL)
		c3a_f = c;
		
		cfcurrent = c;	
	}else{
		
		if (ccurrent != NULL)
		ccurrent->next = c;
		
		if (c3a_root == NULL)
		c3a_root = c;
		
		ccurrent = c;
	}
}

char* c3a_new_value(pp_type_id type)
{
	char* s = (char*) malloc(sizeof(char) * C3A_ETQ_LENGTH);
	switch (type) {
		case INT:
		sprintf(s, "CT%d", c3a_global_var);
		break;
		
		case BOOL:
		sprintf(s, "CT%d", c3a_global_var);
		break;
		
		case ARRAY:
		sprintf(s, "AR%d", c3a_global_arr);
		break;
		
		case GOTO:
		sprintf(s, "ETDEF%d", c3a_global_var);
		break;
	}
	
	if (type != ARRAY)
	c3a_global_var++;
	else
	c3a_global_arr++;
	
	return s;
}

void incr_line()
{
	line_position++;
}

pp_type syna_create_type(pp_type_id type, pp_type next)
{
	pp_type t = (pp_type) malloc(sizeof(struct s_pp_type));
	t->type = type;
	t->next = next;
	
	return t;
}

pp_value env_create_value(pp_type type, int value, pp_value next)
{
	pp_value v = (pp_value) malloc(sizeof(struct s_pp_value));
	v->type = type;
	v->value = value;
	v->members_count = 0;
	v->members = NULL;
	
	return v;
}

pp_var lcl_add_variable(pp_func func, char* name, pp_value value)
{
	pp_var v = (pp_var) malloc(sizeof(struct s_pp_var));
	v->name = strdup(name);
	v->type = value->type;
	v->next = NULL;
	v->value = env_create_value(value->type, 0, NULL);
	if (func->args_current != NULL)
	func->args_current->next = v;
	
	if (func->args == NULL)
	func->args = v;
	
	func->args_current = v;
	
	return v;
}

pp_func env_add_function(char* name,pp_type ret_type, pp_var args)
{
	pp_func f = (pp_func) malloc(sizeof(struct s_pp_func));
	f->name = strdup(name);
	f->ret_type = ret_type;
	f->args = args;
	f->args_current = f->args;
	f->body = NULL;
	if (f_current != NULL)
	f_current->next = f;
	
	if (f_root == NULL)
	f_root = f;
	
	f_current = f;
	
	return f;
}

pp_func env_get_function(char* context_name, int decl)
{
	pp_func f = f_root;
	while (f != NULL && strcmp(f->name, context_name))
	f = f->next;
	
	if (f == NULL && !decl)
	{
		char* s = (char*) malloc(sizeof(char) * ERR_BUFFER_SIZE);
		sprintf(s, "Symbol '%s' not found, return type considered as null", context_name);
		err_display(s);
	}
	
	return f;
}

int env_get_args_number(pp_func f)
{
	int result = 0;
	pp_var v = f->args;
	while(v != NULL)
	{
		result++;
		v = v->next;
	}
	
	return result;
}
//AST

syna_node syna_create_node(int num_childs)
{
	syna_node n = (syna_node) malloc(sizeof(struct s_syna_node));
	n->type = NEMPTY;
	n->value = NULL;
	n->variable = NULL;
	n->function = NULL;
	n->value = NULL;
	n->opi = INONE;
	n->opb = BNONE;
	n->string = NULL;
	n->line_position = line_position;
	n->childs = (syna_node*) malloc(sizeof(struct s_syna_node) * num_childs);
	return n;
}

syna_node syna_opi_node(syna_node member_left, syna_node member_right, syna_opi op)
{
	syna_node n = syna_create_node(2);
	n->type = NOPI;
	n->childs[0] = member_left;
	n->childs[1] = member_right;
	n->value = env_create_value(syna_create_type(INT, NULL), 0, NULL);
	n->opi = op;
	
	return n;
}

syna_node syna_opb_node(syna_node member_left, syna_node member_right, syna_opb op)
{
	syna_node n = syna_create_node(2);
	n->type = NOPB;
	n->childs[0] = member_left;
	n->childs[1] = member_right;
	n->value = env_create_value(syna_create_type(BOOL, NULL), 0, NULL);
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
	n->value = env_create_value(syna_create_type(INT, NULL), value, NULL);
	return n;
}

syna_node syna_var_node(char* name)
{
	syna_node n = syna_create_node(0);
	n->type = NVAR;
	n->string = strdup(name);
	
	return n;
}

syna_node syna_bool_node(int value)
{
	syna_node n = syna_create_node(0);
	n->type = NVALUE;
	n->value = env_create_value(syna_create_type(BOOL, NULL), value, NULL);
	
	return n;
}

syna_node syna_array_node(syna_node member, syna_node index)
{
	syna_node n = syna_create_node(2);
	n->type = NARRAY;
	n->childs[0] = member;
	n->childs[1] = index;
	
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
	n->value = type->value;
	
	return n;
}

syna_node syna_type_node(pp_type_id type, syna_node next)
{
	syna_node n = syna_create_node(1);
	n->type = NTYPE;
	n->childs[0] = next;
	n->value = env_create_value(syna_create_type(type, NULL), 0, NULL);
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
	syna_node n = syna_create_node(1);
	n->type = NFPCALL;
	n->string = strdup(name);
	n->childs[0] = args;
	
	return n;
}

syna_node syna_newarray_node(syna_node type, syna_node expr)
{
	syna_node n = syna_create_node(2);
	n->type = NNA;
	n->childs[0] = type;
	n->childs[1] = expr;
	return n;
}

void syna_link_args_to_func(pp_func func, syna_node args)
{
	switch (args->type) {
		case NBRANCH:
		syna_link_args_to_func(func, args->childs[1]);
		syna_link_args_to_func(func, args->childs[0]);
		break;
		
		case NVDEF:
		syna_execute(args->childs[0], NULL);
		syna_execute(args->childs[1], NULL);
		lcl_add_variable(func, args->childs[0]->string, args->childs[1]->value);
		break;
	}
}

void env_link_arguments(syna_node root, pp_context current_context, pp_context context, pp_func f)
{
	switch (root->type) {
		case NBRANCH:
		env_link_arguments(root->childs[1], current_context, context,f);
		env_link_arguments(root->childs[0], current_context, context,f);
		break;
		
		case NEMPTY:
		break;
		
		default:
		is_af = 1;
		syna_execute(root, current_context);
		is_af = 0;
		c3a_add_line(NULL, "Param", f->args_current->name, root->string, NULL);
		f->args_current = f->args_current->next;
		break;
	}
	
}

void syna_execute(syna_node root, pp_context context)
{
	line_position = root->line_position;
	switch (root->type) {
		case NEMPTY:
		break;
		
		case NROOT:
		syna_execute(root->childs[0], context);
		fun = 1;
		lcl = 1;
		syna_execute(root->childs[1], context);
		lcl = 0;
		fun = 0;
		syna_execute(root->childs[2], context);
		break;
		
		case NOPI:
		{
			
			syna_execute(root->childs[0], context);
			syna_execute(root->childs[1], context);
			root->string = c3a_new_value(INT);
			switch (root->opi) {
				case PL:
				c3a_add_line(NULL, "Pl", root->childs[0]->string, root->childs[1]->string, root->string);
				break;
				
				case MO:
				c3a_add_line(NULL, "Mo", root->childs[0]->string, root->childs[1]->string, root->string);
				break;
				
				case MU:
				c3a_add_line(NULL, "Mu", root->childs[0]->string, root->childs[1]->string, root->string);
				break;
				
			}
			break;
			
			case NOPB:
			{
				is_af = 1;
				if (root->opb != NOT)
				syna_execute(root->childs[0], context);
				
				syna_execute(root->childs[1], context);
				root->string = c3a_new_value(BOOL);
				switch (root->opb) {
					case OR:
					c3a_add_line(NULL, "Or", root->childs[0]->string, root->childs[1]->string, root->string);
					break;
					
					case AND:
					c3a_add_line(NULL, "And", root->childs[0]->string, root->childs[1]->string, root->string);
					break;
					
					case NOT:
					c3a_add_line(NULL, "Not", root->childs[1]->string, NULL, root->string);
					break;
					
					case LT:{
						c3a_add_line(NULL, "Lt", root->childs[0]->string, root->childs[1]->string, root->string);
					}
					break;
					
					case EQ:
					{
						char* s = (char*) malloc(sizeof(char) * C3A_ETQ_LENGTH); 
						s = c3a_new_value(BOOL);
						c3a_add_line(NULL, "Mo", root->childs[0]->string, root->childs[1]->string, root->string);
						char* ss = (char*) malloc(sizeof(char) * C3A_ETQ_LENGTH); 
						
						ss = c3a_new_value(BOOL);
						c3a_add_line(NULL, "Not", root->string, NULL, ss);
						root->string = ss;
					}
					break;
				}
			}
			break;
			
			case NPBA:
			syna_execute(root->childs[0], context);
			root->string = c3a_new_value(INT);
			c3a_add_line(NULL, "Af", root->string, root->childs[0]->string, NULL); 
			break;
			
			case NVALUE:
			root->string = c3a_new_value(INT);
			char* s = (char*) malloc(sizeof(char) * C3A_ETQ_LENGTH);
			sprintf(s, "%d", root->value->value);
			c3a_add_line(NULL, "Afc", s, NULL, root->string);
			break;
			
			case NVAR:
			{
				char* s = strdup(root->string);
				root->variable = (pp_var) malloc(sizeof(struct s_pp_var));
				root->variable->name = s;
				if (!lcl)
				{
					root->string = c3a_new_value(INT);
					if (is_af)
					c3a_add_line(NULL, "Af", root->string, s, NULL);
				}
				
			}
			break;
			
			case NARRAY:
			{
				syna_execute(root->childs[1], context);
				if (root->childs[0]->type == NARRAY)
				{
					dim++;
					syna_execute(root->childs[0], context);
					dim--;
					
				}
				else
				root->string = root->childs[0]->string;
				
				
				
				
				if (dim == 0 && root->childs[0]->type == NARRAY)
				root->string = c3a_new_value(INT);
				else if (dim > 0 || root->childs[0]->type != NARRAY)
				root->string = c3a_new_value(ARRAY);
				
				c3a_add_line(NULL, "Ind", root->childs[0]->string, root->childs[1]->string, root->string);
			}
			break;
			
			case NNA:
			{
				char* s = (char*) malloc(sizeof(char) * C3A_ETQ_LENGTH);
				char* ss = (char*) malloc(sizeof(char) * C3A_ETQ_LENGTH);
				char* n = (char*) malloc(sizeof(char) * C3A_ETQ_LENGTH);
				if (root->string != NULL)
				sprintf(s, "CT%s", root->string);
				else
				{
					s = c3a_new_value(ARRAY);
					sprintf(ss, "CT%s", s);
				}
				if (root->childs[1]->value != NULL)
				sprintf(n, "%d", root->childs[1]->value->value);
				else
				n = root->childs[1]->string;
				
				if (root->string != NULL)
				c3a_add_line(NULL, "Afc", n, NULL, s);
				else
				c3a_add_line(NULL, "Afc", n, NULL, ss);
				
				root->string = s;
			}
			
			break;
			
			case NBRANCH:
			syna_execute(root->childs[0], context);
			syna_execute(root->childs[1], context);
			break;
			
			case NITE:
			{
				char* e = c3a_new_value(GOTO);
				char* g = c3a_new_value(GOTO);
				is_af = 1;
				syna_execute(root->childs[0], context);
				is_af = 0;
				c3a_add_line(NULL, "Jz", root->childs[0]->string, NULL, e);
				syna_execute(root->childs[1], context);
				c3a_add_line(NULL, "Jp", NULL, NULL, g);
				c3a_add_line(e, "Sk", NULL, NULL, NULL);
				syna_execute(root->childs[2], context);
				c3a_add_line(g, "Sk", NULL, NULL, NULL);
			}
			break;
			
			case NWD:
			{
				char* w = c3a_new_value(GOTO);
				char* e = c3a_new_value(GOTO);
				c3a_add_line(w, "Sk", NULL, NULL, NULL);
				syna_execute(root->childs[0], context);
				c3a_add_line(NULL, "Jz", root->childs[0]->string, NULL, e);
				syna_execute(root->childs[1], context);
				c3a_add_line(NULL, "Jp", NULL, NULL, w);
				c3a_add_line(e, "Sk", NULL, NULL, NULL);
			}
			break;
			
			case NAAF:	
			syna_execute(root->childs[0], context);
			syna_execute(root->childs[1], context);
			c3a_add_line(NULL, "AfInd", root->childs[0]->string, root->childs[0]->childs[1]->string, root->childs[1]->string);
			break;
			
			case NVAF:
			if (root->childs[1]->type == NNA)
			root->childs[1]->string = root->childs[0]->string;
			is_af = 1;
			syna_execute(root->childs[1], context);
			is_af = 0;
			syna_execute(root->childs[0], context);
			if (root->childs[1]->type != NNA)
			c3a_add_line(NULL, "Af", root->childs[0]->variable->name, root->childs[1]->string, NULL);
			break;
			
			case NSKIP:
			c3a_add_line(NULL, "Sk", NULL, NULL, NULL);
			break;
			
			case NVDEF:
			break;
			
			case NTYPE:
			break;
			case NPDEF:
			case NFDEF:
			{
				char* s = (char*) malloc(sizeof(char) * C3A_ETQ_LENGTH);
				sprintf(s, "ET%s", root->string);
				c3a_add_line(s, "Sk", NULL, NULL, NULL);
				root->function = env_add_function(root->string, NULL, root->variable);
				syna_link_args_to_func(root->function, root->childs[0]);
			}
			break;
			
			case NPBODY:
			case NFBODY:
			syna_execute(root->childs[0], context);
			root->childs[0]->function->body = root->childs[2];
			syna_execute(root->childs[2], context);
			c3a_add_line(NULL, "Ret", NULL, NULL, NULL);
			break;
			
			case NFPCALL:
			{
				pp_func f = env_get_function(root->string, 0);
				int n = env_get_args_number(f);
				char* ns = (char*) malloc(sizeof(char) * C3A_ETQ_LENGTH);
				f->args_current = f->args;
				env_link_arguments(root->childs[0], NULL,NULL, f);
				char* s = (char*) malloc(sizeof(char) * C3A_ETQ_LENGTH);
				sprintf(s, "ET%s", root->string);
				sprintf(ns, "%d", n);
				c3a_add_line(NULL, "Call", s, ns, NULL);
			}
			break;
		}
	}
}

void err_display(char* s)
{
	fprintf(stderr, "***ERROR l.%d : %s***\n", line_position, s);
	exe_stop();
	total_errors++;
}

void err_report()
{
	if (!total_errors)
	printf("No error found.\n");
	else
	printf("%d error%s found.\n", total_errors, (total_errors > 1 ? "s" : ""));
}
void exe_stop()
{
	fprintf(stderr, "Execution stopped.\n");
	exit(-1);
}

void env_report()
{
	c3a_add_line(NULL, "St", NULL, NULL, NULL);
	while (c3a_root != NULL)
	{
		fprintf(stdout, "%s\t:%s\t:%s\t:%s\t:%s\t\n", c3a_root->etq, c3a_root->op, c3a_root->arg1, c3a_root->arg2, c3a_root->dst);
		c3a_root = c3a_root->next;
	}
	
	while (c3a_f != NULL)
	{
		fprintf(stdout, "%s\t:%s\t:%s\t:%s\t:%s\t\n", c3a_f->etq, c3a_f->op, c3a_f->arg1, c3a_f->arg2, c3a_f->dst);
		c3a_f = c3a_f->next;
	}
}
