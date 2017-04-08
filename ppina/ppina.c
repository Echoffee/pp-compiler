#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppina.h"

#define ERR_BUFFER_SIZE 256

pp_func f_context = NULL;
pp_func f_root = NULL;
pp_func f_current = NULL;

int in_fuction_call = 0;
int var_declaration = 0;
int total_errors = 0;
int line_position = 1;

pp_var current_arg = NULL;

pp_context main_context = NULL;

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
	v->next = next;
	v->members_count = 0;
	v->members = NULL;
	
	return v;
}

pp_value env_create_array(int size, pp_type type)
{
	pp_value value = env_create_value(syna_create_type(ARRAY, type), 0, NULL);
	value->members = (pp_value*) malloc(sizeof(pp_value) * size);
	value->members_count = size;
	for (int i = 0; i < size; i++)
		value->members[i] = env_create_value(type, 0, NULL);

	return value;
}

pp_var exe_add_variable(char* name, pp_context context, pp_type type)
{
	pp_var v = (pp_var) malloc(sizeof(struct s_pp_var));
	v->name = strdup(name);
	v->type = NULL;
	v->next = NULL; //Only value matters
	v->value = env_create_value(type, 0, NULL);
	
	if (context->current_context != NULL)
		context->current_context->next = v;
		
	if (context->context == NULL)
		context->context = v;
		
	context->current_context = v;
	
	return v;
}

pp_var env_add_variable(char* name, pp_type type)
{
	pp_var v = (pp_var) malloc(sizeof(struct s_pp_var));
	v->name = strdup(name);
	v->type = type;
	v->next = NULL;
	v->value = env_create_value(type, 0, NULL);
	
	if (f_context->context_current != NULL)
		f_context->context_current->next = v;
		
	if (f_context->context == NULL)
		f_context->context = v;
		
	f_context->context_current = v;
	
	return v;
}

pp_var lcl_add_variable(pp_func func, char* name, pp_value value)
{
	pp_var v = (pp_var) malloc(sizeof(struct s_pp_var));
	v->name = strdup(name);
	v->type = value->type;
	v->next = NULL;
	v->value = NULL;
	if (func->args_current != NULL)
		func->args_current->next = v;
		
	if (func->args == NULL)
		func->args = v;
		
	func->args_current = v;
	
	return v;
}

pp_func env_add_function(char* name, pp_type ret_type, pp_var args)
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
	
	return f;
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
	//env_add_function("main_program", syna_create_type(NONE, NULL), NULL);
	//env_change_context("main_program", 0);
}

/*void env_change_context(char* context_name, int decl)
{
		pp_func f = f_root;
		while (f != NULL && strcmp(f->name, context_name))
			f = f->next;
			
		if (f == NULL && !decl)
			fprintf(stderr, "ERROR : Context '%s' not found\n", context_name);
			
		f_context = f;
}*/

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
	
	return result - 1;
}

pp_var exe_get_variable(char* name, pp_context context)
{
	pp_var v = context->context;
	while (v != NULL && strcmp(v->name, name))
	{
		v = v->next;
	}
	
	if (v == NULL)
	{
		fprintf(stderr, "%s\n", name);
		err_display("Variable not found :(");
	}
	
	return v;
}

pp_var env_get_variable(char* name, int decl, pp_var args)
{
	pp_func c = f_context;
	char* prev_context = strdup(c->name);
	pp_var v = c->args;
	int i = 0;
	while (v != NULL && strcmp(v->name, name))
	{
			v = v->next;
			i++;
	}
	
	if (v != NULL)
	{
		if (args == NULL)
			return v;
		else
		{
			pp_var r = args;
			while (i > 0)
			{
				r = r->next;
				i--;
			}
			
			return r;
		}
	}
	
	v = c->context;
	while (v != NULL && strcmp(v->name, name))
		v = v->next;
		
	if (v != NULL)
		return v;
	
	//env_change_context("main_program", decl);
	v = f_context->context;
	while (v != NULL && strcmp(v->name, name))
		v = v->next;
	
	if (v == NULL && !decl)
	{
		char* s = (char*) malloc(sizeof(char) * ERR_BUFFER_SIZE);
		sprintf(s, "Variable '%s' not found (current context : '%s')\n", name, prev_context);
		err_display(s);
	}
		
	//env_change_context(prev_context, decl);
	if (v == NULL)
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
	//n->variable = env_get_variable(name);
	n->string = strdup(name);
	//n->value->type = n->variable->type;
	
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
	//Find a way to get an object from an array 1d+
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
	//n->value->type = syna_create_type(type, NULL); //incorrect (1d only)
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
			env_link_arguments(root->childs[0], current_context, context,f);
			env_link_arguments(root->childs[1], current_context, context,f);
			break;
			
		default:
			syna_execute(root, current_context);
			//context->current_context->value = root->value;
			//context->current_context->type = root->value->type;
			pp_var n = exe_add_variable(f->args_current->name, context, root->value->type);
			n->scope = LOCAL;
			//pp_var n = (pp_var) malloc(sizeof(struct s_pp_var));
			//pp_value vv = (pp_value) malloc(sizeof(struct s_pp_value));
			//vv->value = root->value->value;
			//vv->type = root->value->type;
			int val = root->value->value;
			n->value->value = val;
			f->args_current = f->args_current->next;
		break;
	}
	
}

char* err_display_type(pp_type type)
{
	char* result = (char*) malloc(sizeof(char) * ERR_BUFFER_SIZE);
	while (type != NULL)
	{
		char* s;
		switch (type->type) {
			case NONE:
				s = strdup("null");
			break;
			
			case INT:
				s = strdup("integer");
			break;
			
			case BOOL:
				s = strdup("boolean");
			break;
			
			case ARRAY:
				s = strdup("array of");
			break;
		}
		
		sprintf(result, "%s %s", result, s);
		type = type->next;
	}
	
	return result;
}

int err_check_type_rec(pp_type n, pp_type type)
{
	if (n->type != type->type)
		return 0;
	
	if (type->type == ARRAY)
		return err_check_type_rec(n->next, type->next);
		
	return 1;
	
}

int err_check_type(pp_type n, pp_type type)
{
	int e = err_check_type_rec(n, type);
	if (!e)
	{
		char* s = (char*) malloc(sizeof(char) * ERR_BUFFER_SIZE);
		sprintf(s, "Incorrect type : expected type%s but got%s.", err_display_type(type), err_display_type(n));
		err_display(s);
	}
	
	return e;
}

void err_check_single_argument(syna_node arg, pp_func f, int index, int max)
{
	pp_var v = f->args;
	int i = 0;
	while (i < max - index)
	{
		i++;
		v = v->next;
	}
	
	err_check_type(arg->value->type, v->type);
}

int err_check_arguments(syna_node arg_node, pp_context context, pp_func f, int rank, int rank_max, int check)
{
	if (arg_node->type == NEMPTY)
		rank--;
	
	int b1 = (rank == rank_max?1:0);
	int b2 = 0;
	
	if (rank > rank_max)
	{
		char* s = (char*) malloc(sizeof(char) * ERR_BUFFER_SIZE);
		sprintf(s, "Too many arguments : expected %d arguments for %s.", rank_max + 1, f->name);
		err_display(s);
		return 1;
	}
	
	switch (arg_node->type) {
		case NBRANCH:
			b1 = err_check_arguments(arg_node->childs[0], context, f, rank, rank_max, check);
			rank++;
			b2 = err_check_arguments(arg_node->childs[1], context, f, rank, rank_max, check);
		break;
		case NEMPTY:
			return b1;
		break;
		default:
			if (check)
				syna_check(arg_node, context);
			else
				syna_execute(arg_node, context);
			
			err_check_single_argument(arg_node, f, rank, rank_max);
		break;
	}
	
	return b1 + b2;
}

pp_context exe_create_context()
{
	pp_context c = (pp_context) malloc(sizeof(struct s_pp_context));
	c->context = NULL;
	c->current_context = NULL;
	c->return_value = NULL;
	
	if (main_context == NULL)
		main_context = c;
	
	return c;
}

pp_context exe_copy_context(pp_context context)
{
	pp_context c = exe_create_context();
	pp_var v = context->context;
	while (v != NULL)
	{
		if (v->scope != LOCAL)
		{
		pp_var vd = (pp_var) malloc(sizeof(struct s_pp_var));
		vd->name = v->name;
		vd->type = v->type;
		vd->scope = v->scope;
		vd->next = NULL;
		vd->value = v->value;
			//vd->value = env_create_value(v->value->type, v->value->value, NULL);
			//fprintf(stderr, "local : %s var_declaration\n", v->name);
			
		if (c->current_context != NULL)
			c->current_context->next = vd;
		
		if (c->context == NULL)
			c->context = vd;
			
		c->current_context = vd;
	}
		v = v->next;
	}
	
	return c;
}

void syna_execute(syna_node root, pp_context context)
{
	line_position = root->line_position;
	switch (root->type) {
		case NEMPTY:
			//eh
			break;
		
		case NROOT:
			var_declaration = 1;
			syna_execute(root->childs[0], context);
			var_declaration = 0;
			syna_execute(root->childs[1], context);
			syna_execute(root->childs[2], context);
			break;
		
		case NOPI:
			//Check if both values are of type Integer 
			syna_execute(root->childs[0], context);
			syna_execute(root->childs[1], context);
			if (	err_check_type(root->childs[0]->value->type, syna_create_type(INT, NULL))
				&&	err_check_type(root->childs[1]->value->type, syna_create_type(INT, NULL)))
				{
					int a = root->childs[0]->value->value;
					int b = root->childs[1]->value->value;
					//fprintf(stderr, "value of a = %d\n", a);
					//fprintf(stderr, "value of b = %d\n", b);
					int c;
					switch (root->opi) {
						case PL:
							c = a + b;
							break;
						
						case MO:
							c = a - b;
							break;
						
						case MU:
							c = a * b;
							break;
					}
					
					pp_value v = env_create_value(syna_create_type(INT, NULL), c, NULL);
					root->value = v;
				}
		break;
		
		case NOPB:
			//Check if both values are of type Boolean
			if (root->opb != NOT)
				syna_execute(root->childs[0], context);
			
			syna_execute(root->childs[1], context);
			int err_status = 1;
			err_status = (root->childs[1]->value != NULL ? 1 : 0);
			switch (root->opb) {
				case OR:
				case AND:
					err_status *= err_check_type(root->childs[0]->value->type, syna_create_type(BOOL, NULL));
				case NOT:
					err_status *= err_check_type(root->childs[1]->value->type, syna_create_type(BOOL, NULL));			
				break;
				
				case LT:
				case EQ:
					err_status *= err_check_type(root->childs[0]->value->type, syna_create_type(INT, NULL))
								* err_check_type(root->childs[1]->value->type, syna_create_type(INT, NULL));
				break;			
			}
			
			if (err_status)
			{
				int b = root->childs[1]->value->value;
				int a = 0;
				if (root->opb != NOT)
					a = root->childs[0]->value->value;
				
				int c;
				switch (root->opb) {		//TODO: Optimization
					case OR:
						c = a + b;
						break;
						
					case AND:
						c = a * b;
						break;
						
					case NOT:
						c = (b == 0 ? 1 : 0);
						break;
					
					case LT:
						c = (a <= b ? 1 : 0);
						break;
						
					case EQ:
						c = (a == b ? 1 : 0);
						break;
				}
				
				pp_value v = env_create_value(syna_create_type(BOOL, NULL), c, NULL);
				root->value = v;
			}else{
				err_display("Cannot evalue expression (are members correctly initialized ?)");
				root->value == NULL;
			}
		break;
		
		case NPBA:
		   syna_execute(root->childs[0], context);
		   root->value = root->childs[0]->value;
		break;
		
		case NVALUE:
			//Check type ?
			//root->value = env_create_value(root->value->type, root->value->value, NULL);
		break;
		
		case NVAR:
			{
			//Check type
			//fprintf(stderr, "looking for %s\n", root->string);
			//pp_var v = env_get_variable(root->string, var_declaration, context);
			pp_var v = exe_get_variable(root->string, context);
			root->variable = v;
			root->value = v->value;
			//fprintf(stderr, "okvar : %s : %d\n", root->string, root->value->value);
			}
		break;
		
		case NARRAY:	//TODO: aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
			syna_execute(root->childs[1], context);//index
			err_check_type(root->childs[1]->value->type, syna_create_type(INT, NULL));
			syna_execute(root->childs[0], context);//array-side
			//root->value->type = syna_create_type(ARRAY, root->childs[0]->value->type);
			root->value->type = root->childs[0]->value->type->next;
			if (root->childs[0]->value == NULL)
				err_display("Value has not been initialized");
				
			if (root->childs[0]->value->members_count <= root->childs[1]->value->value)
			{
				char* s = (char*) malloc(sizeof(char) * ERR_BUFFER_SIZE);
				sprintf(s, "Index is out of bounds, array is of size %d but index is %d%s", root->childs[0]->value->members_count
				, root->childs[1]->value->value, (root->childs[0]->value->members_count == 0 ? " (is the array correctly initialized ?)": ""));
				err_display(s);
			}
			else
			{
				root->value = root->childs[0]->value->members[root->childs[1]->value->value];
			}
			//fprintf(stderr, "%d\n", root->childs[0]->value->type->type);
		break;
		
		case NNA:
			//Check if index is of type integer
			syna_execute(root->childs[0], context); //Type
			syna_execute(root->childs[1], context); //Size
			if (err_check_type(root->childs[1]->value->type, syna_create_type(INT, NULL)))
			{
				root->value->type = syna_create_type(ARRAY, root->childs[0]->value->type);
				root->value = env_create_array(root->childs[1]->value->value, root->childs[0]->value->type);
			}
		break;
		
		case NBRANCH:
			syna_execute(root->childs[0], context);
			syna_execute(root->childs[1], context);
		break;
		
		case NITE:
			//Check if condition is of type Boolean
			syna_execute(root->childs[0], context);
			syna_check(root->childs[1], context);
			syna_check(root->childs[2], context);
			if (err_check_type(root->childs[0]->value->type, syna_create_type(BOOL, NULL)))
			{
				if (root->childs[0]->value->value > 0)
					syna_execute(root->childs[1], context);
				else
					syna_execute(root->childs[2], context);
			}
		break;
		
		case NWD:
			//Check if condition is of type Boolean
			syna_execute(root->childs[0], context);
			syna_check(root->childs[1], context);
			if (root->childs[0]->value != NULL && root->childs[0]->value->type != NULL && err_check_type(root->childs[0]->value->type, syna_create_type(BOOL, NULL)))
				while (root->childs[0]->value != NULL && root->childs[0]->value->value > 0)
				{
					syna_execute(root->childs[1], context);
					syna_execute(root->childs[0], context);
				}
			
		break;
		
		case NAAF:	//TODO: do you like my car ?
			//Check if value and destination are of same type
			syna_execute(root->childs[0], context);
			syna_execute(root->childs[1], context);
			if (err_check_type(root->childs[1]->value->type, root->childs[0]->value->type))
			{
				if (root->childs[1]->value->type->type != ARRAY)
					root->childs[0]->value->value = root->childs[1]->value->value;
				else
				{
					root->childs[0]->value->members = root->childs[1]->value->members;
					root->childs[0]->value->members_count = root->childs[1]->value->members_count;
				}
			}
		break;
		
		case NVAF:
			//Check if value and destination are of same type
			syna_execute(root->childs[1], context);
			syna_execute(root->childs[0], context);
			if (err_check_type(root->childs[1]->value->type, root->childs[0]->value->type))
			{
				root->childs[0]->variable->value = root->childs[1]->value;
			}
			
			break;
		
		case NSKIP:
			//eh	
			break;
		
		case NVDEF:
			//root->childs[0]->variable = env_get_variable(root->childs[0]->string, var_declaration, context);
			root->childs[0]->variable = exe_add_variable(root->childs[0]->string, context, root->value->type);
			if (root->childs[1] != NULL)
				syna_execute(root->childs[1], context);
				
			root->childs[0]->variable->type = root->value->type;
			break;
		
		case NTYPE:
			if (root->childs[0] != NULL)
			{
				syna_execute(root->childs[0], context);
				//root->value = env_create_value(syna_create_type())
				root->value->type->next = root->childs[0]->value->type;
			}
		break;
		
		case NPDEF:
			root->function = env_add_function(root->string, syna_create_type(NONE, NULL), root->variable);
			//env_change_context(root->string, 0);
			syna_link_args_to_func(root->function, root->childs[0]);	//define args
		break;
		
		case NFDEF:
			syna_execute(root->childs[1], context); //define ret type
			root->value = root->childs[1]->value;
			root->function = env_add_function(root->string, root->value->type, root->variable);
			//env_change_context(root->string, 0);
			syna_link_args_to_func(root->function, root->childs[0]);	//define args
		break;
	
		case NPBODY:
		var_declaration = 1;
		syna_execute(root->childs[0], context); //Procedure declaration
		//Context already changed
		syna_execute(root->childs[1], context); //Add local variables
		//When called, must execute root->childs[2]
		var_declaration = 0;
		root->childs[0]->function->body = root->childs[2];
		
		//syna_check(root->childs[2], context);
		//env_change_context("main_program", 0);
		break;
		
		case NFBODY:
		var_declaration = 1;
		syna_execute(root->childs[0], context); //Function declaration
		//Context already changed
		syna_execute(root->childs[1], context); //Add local variables
		//When called, must execute root->childs[2]
		var_declaration = 0;
		root->childs[0]->function->body = root->childs[2];
		//env_add_variable(root->childs[0]->string, root->childs[0]->value->type); // Add return value
		//syna_check(root->childs[2], context);
		break;
		
		case NFPCALL:
		{	//NOTE: Somehow the syna_check is crashing, don't , contextknow why, but this might be the last step for ppina
			//syna_execute(root->childs[0], context);
			pp_func f = env_get_function(root->string, 0);
				root->value = env_create_value(f->ret_type, 0, NULL);
				if (!err_check_arguments(root->childs[0], context, f, 0, env_get_args_number(f), 0))
				{
					char* s = (char*) malloc(sizeof(char) * ERR_BUFFER_SIZE);
					sprintf(s, "Too few arguments for %s", f->name);
					err_display(s);
				}else{
						
					//ina_execute(f, root->childs[0]);
					//should syna_execute(with new args)
					pp_context new_context = exe_copy_context(context);
					f->args_current = f->args;
					env_link_arguments(root->childs[0], context, new_context, f);
					pp_var ret = exe_add_variable(f->name, new_context, f->ret_type);
					ret->scope = LOCAL;
					fprintf(stdout, "self before exec\n");
					env_display(context);
					syna_execute(f->body, new_context);
					fprintf(stdout, "self after exec\n");
					env_display(context);
					//root->value = env_create_value(f->ret_type, vvv->value->value, NULL);
					root->value = ret->value;
					////env_change_context("main_program", 0);
				}
		}
		break;
	}
}

void syna_check(syna_node root, pp_context context)
{
	line_position = root->line_position;
	switch (root->type) {
		case NEMPTY:
			//eh
			break;
		
		case NROOT:
			var_declaration = 1;
			syna_check(root->childs[0], context);
			var_declaration = 0;
			syna_check(root->childs[1], context);
			syna_check(root->childs[2], context);
			break;
		
		case NOPI:
			//Check if both values are of type Integer 
			syna_check(root->childs[0], context);
			syna_check(root->childs[1], context);
			err_check_type(root->childs[0]->value->type, syna_create_type(INT, NULL));
			err_check_type(root->childs[1]->value->type, syna_create_type(INT, NULL));
		break;
		
		case NOPB:
			//Check if both values are of type Boolean
			if (root->opb != NOT)
				syna_check(root->childs[0], context);
			
			syna_check(root->childs[1], context);
			int err_status = 1;
			switch (root->opb) {
				case OR:
				case AND:
					err_status = err_check_type(root->childs[0]->value->type, syna_create_type(BOOL, NULL));
				case NOT:
					err_status = err_check_type(root->childs[1]->value->type, syna_create_type(BOOL, NULL));			
				break;
				
				case LT:
				case EQ:
					err_status = err_check_type(root->childs[0]->value->type, syna_create_type(INT, NULL))
								* err_check_type(root->childs[1]->value->type, syna_create_type(INT, NULL));
				break;			
			}
		break;
		
		case NPBA:
		   syna_check(root->childs[0], context);
		   root->value->type = root->childs[0]->value->type;
		   root->value = root->childs[0]->value;
		break;
		
		case NVALUE:
			//Check type ?
		break;
		
		case NVAR:
			{
			//Check type
			pp_var v = exe_get_variable(root->string, context);
			root->value = env_create_value(v->value->type, v->value->value, NULL);	
			}
		break;
		
		case NARRAY:	//TODO: aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
			syna_check(root->childs[1], context);//index
			err_check_type(root->childs[1]->value->type, syna_create_type(INT, NULL));
			syna_check(root->childs[0], context);//array-side
			//root->value->type = syna_create_type(ARRAY, root->childs[0]->value->type);
			root->value->type = root->childs[0]->value->type->next;
			//fprintf(stderr, "%d\n", root->childs[0]->value->type->type);
		break;
		
		case NNA:
			//Check if index is of type integer
			syna_check(root->childs[0], context);
			syna_check(root->childs[1], context);
			err_check_type(root->childs[1]->value->type, syna_create_type(INT, NULL));
			root->value->type = syna_create_type(ARRAY, root->childs[0]->value->type);
		break;
		
		case NBRANCH:
			syna_check(root->childs[0], context);
			syna_check(root->childs[1], context);
		break;
		
		case NITE:
			//Check if condition is of type Boolean
			syna_check(root->childs[0], context);
			err_check_type(root->childs[0]->value->type, syna_create_type(BOOL, NULL));
			syna_check(root->childs[1], context);
			syna_check(root->childs[2], context);
		break;
		
		case NWD:
			//Check if condition is of type Boolean
			syna_check(root->childs[0], context);
			syna_check(root->childs[1], context);
			err_check_type(root->childs[0]->value->type, syna_create_type(BOOL, NULL));
		break;
		
		case NAAF:	//TODO: do you like my car ?
			//Check if value and destination are of same type
			syna_check(root->childs[0], context);
			syna_check(root->childs[1], context);
			err_check_type(root->childs[1]->value->type, root->childs[0]->value->type);
		break;
		
		case NNVAR:
			//Check if value and destination are of same type

		break;
		
		case NVAF:
			//Check if value and destination are of same type
			syna_check(root->childs[1], context);
			syna_check(root->childs[0], context);
			err_check_type(root->childs[1]->value->type, root->childs[0]->value->type);
			break;
		
		case NSKIP:
			//eh	
			break;
		
		case NEXPR:
			
		break;
		
		case NVDEF:
			root->childs[0]->variable = env_get_variable(root->childs[0]->string, var_declaration, NULL);
			if (root->childs[1] != NULL)
				syna_check(root->childs[1], context);
				
			root->childs[0]->variable->type = root->value->type;
			break;
		
		case NTYPE: 
			if (root->childs[0] != NULL)
			{
				syna_check(root->childs[0], context);
				root->value->type->next = root->childs[0]->value->type;
			}
		break;
		
		case NFPCALL:
		{
			syna_check(root->childs[0], context);
			pp_func f = env_get_function(root->string, 0);
			if (f != NULL)
			{
				root->value = env_create_value(f->ret_type, 0, NULL);
				if (!err_check_arguments(root->childs[0], context, f, 0, env_get_args_number(f), 1))
				{
					char* s = (char*) malloc(sizeof(char) * ERR_BUFFER_SIZE);
					sprintf(s, "Too few arguments for %s", f->name);
					err_display(s);
				}
			}else{
				root->value->type = syna_create_type(NONE, NULL);
			}
		}
		break;
	}
}

/*void ina_execute(pp_func f, syna_node args)
{
	/*pp_var v = f->args;
	while (v != NULL)
	{
		//NOTE: 0141am Need to link the args to local variables before executing f.
		// The hard part is to determine which leaf of the syna_node is for which argument
		// Maybe modyfing err_check_arguments is needed, absolutely not sure about that.
		// This is the hardest part, after that, everything will be 大丈夫.
	}
	env_link_args_to_vars(args, f);
	syna_execute(f->body);
	fprintf(stderr, "ina_ex bai\n");
}*/

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

void env_display_value(pp_value v, int root)
{
	if (root)
		printf("Value : ");
		if (v == NULL || v->type == NULL)
			printf("undefined;\n");
		else{
			switch (v->type->type) {
				case INT:
					printf("%d", v->value);
					break;
					
					case BOOL:
					printf("%s", (v->value > 0 ? "TRUE" : "FALSE"));
					break;
					
					case ARRAY:
					printf("size : %d [", v->members_count);
					for (int i = 0; i < v->members_count; i++)
					{
						env_display_value(v->members[i], 0);
						if (i < v->members_count - 1)
						printf(", ");
					}
					
					printf("]");
					break;
				}
	
				if (root)
				printf(";\n");
	}
}

void env_display(pp_context context)
{
	printf("===== ENV =====\n");
	//pp_func f = f_root;
	//while(f != NULL)
	//{
		pp_var v_root = context->context;
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
				
				printf(" %s\n", s);
				t_current = t_current->next;
			}
			
			//printf(" in context '%s'.\n", f->name);
			env_display_value(v_root->value, 1);
			v_root = v_root->next;
		}
		
		//f = f->next;
	//}
	
	printf("===== END =====\n");
}

void exe_stop()
{
	fprintf(stderr, "Execution stopped.\n");
	exit(-1);
}

void env_report()
{
	//env_change_context("main_program", 0);
	env_display(main_context);
}