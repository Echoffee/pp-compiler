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
	env_change_context("main_program", 0);
}

void env_change_context(char* context_name, int decl)
{
		pp_func f = f_root;
		while (f != NULL && strcmp(f->name, context_name))
			f = f->next;
			
		if (f == NULL && !decl)
			fprintf(stderr, "ERROR : Context '%s' not found\n", context_name);
			
		f_context = f;
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
	
	return result - 1;
}

pp_var env_get_variable(char* name, int decl)
{
	pp_func c = f_context;
	pp_var v = c->args;
	while (v != NULL && strcmp(v->name, name))
		v = v->next;
		
	if (v != NULL)
		return v;
	
	v = c->context;
	while (v != NULL && strcmp(v->name, name))
		v = v->next;
		
	if (v != NULL)
		return v;
	
	env_change_context("main_program", decl);
	v = f_context->context;
	while (v != NULL && strcmp(v->name, name))
		v = v->next;
	
	if (v == NULL && !decl)
		fprintf(stderr, "ERROR : Variable '%s' not found (current context : '%s')\n", name, c->name);
		
	f_context = c;
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
	n->ivalue = 0;
	n->value_type = NONE;
	n->variable = NULL;
	n->function = NULL;
	n->value = NULL;
	n->ref = NULL;
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
	n->ivalue = value;
	n->value_type = syna_create_type(INT, NULL);
	
	return n;
}

syna_node syna_var_node(char* name)
{
	syna_node n = syna_create_node(0);
	n->type = NVAR;
	//n->variable = env_get_variable(name);
	n->string = strdup(name);
	//n->value_type = n->variable->type;
	
	return n;
}

syna_node syna_bool_node(int value)
{
	syna_node n = syna_create_node(0);
	n->type = NVALUE;
	n->ivalue = value;
	n->value_type = syna_create_type(BOOL, NULL);
	
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
			syna_execute(args->childs[0]);
			syna_execute(args->childs[1]);
			lcl_add_variable(func, args->childs[0]->string, args->childs[1]->value_type);
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
	
	err_check_type(arg->value_type, v->type);
}

int err_check_arguments(syna_node arg_node, pp_func f, int rank, int rank_max)
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
			b1 = err_check_arguments(arg_node->childs[0], f, rank, rank_max);
			rank++;
			b2 = err_check_arguments(arg_node->childs[1], f, rank, rank_max);
		break;
		case NEMPTY:
			return b1;
		break;
		default:
			syna_execute(arg_node);
			err_check_single_argument(arg_node, f, rank, rank_max);
		break;
	}
	
	return b1 + b2;
}

void syna_execute(syna_node root)
{
	line_position = root->line_position;
	switch (root->type) {
		case NEMPTY:
			//eh
			break;
		
		case NROOT:
			var_declaration = 1;
			syna_execute(root->childs[0]);
			var_declaration = 0;
			syna_execute(root->childs[1]);
			syna_execute(root->childs[2]);
			break;
		
		case NOPI:
			//Check if both values are of type Integer 
			syna_execute(root->childs[0]);
			syna_execute(root->childs[1]);
			if (	err_check_type(root->childs[0]->value_type, syna_create_type(INT, NULL))
				&&	err_check_type(root->childs[1]->value_type, syna_create_type(INT, NULL)))
				{
					int a = root->childs[0]->value->value;
					int b = root->childs[1]->value->value;
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
				syna_execute(root->childs[0]);
			
			syna_execute(root->childs[1]);
			int err_status = 1;
			err_status = (root->childs[0]->value != NULL && root->childs[1]->value != NULL ? 1 : 0);
			switch (root->opb) {
				case OR:
				case AND:
					err_status *= err_check_type(root->childs[0]->value_type, syna_create_type(BOOL, NULL));
				case NOT:
					err_status *= err_check_type(root->childs[1]->value_type, syna_create_type(BOOL, NULL));			
				break;
				
				case LT:
				case EQ:
					err_status *= err_check_type(root->childs[0]->value_type, syna_create_type(INT, NULL))
								* err_check_type(root->childs[1]->value_type, syna_create_type(INT, NULL));
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
		   syna_execute(root->childs[0]);
		   root->value_type = root->childs[0]->value_type;
		   root->value = root->childs[0]->value;
		break;
		
		case NVALUE:
			//Check type ?
			root->value = env_create_value(root->value_type, root->ivalue, NULL);
		break;
		
		case NVAR:
			{
			//Check type
			pp_var v = env_get_variable(root->string, var_declaration);
			root->variable = v;
			root->value_type = v->type;	
			root->value = v->value;
			}
		break;
		
		case NARRAY:	//TODO: aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
			syna_execute(root->childs[1]);//index
			err_check_type(root->childs[1]->value_type, syna_create_type(INT, NULL));
			syna_execute(root->childs[0]);//array-side
			//root->value_type = syna_create_type(ARRAY, root->childs[0]->value_type);
			root->value_type = root->childs[0]->value_type->next;
			if (root->childs[0]->value == NULL)
				err_display("Value has not been initialized");
				
			if (root->childs[0]->value->members_count <= root->childs[1]->ivalue)
			{
				char* s = (char*) malloc(sizeof(char) * ERR_BUFFER_SIZE);
				sprintf(s, "Index is out of bounds, array is of size %d but index is %d%s", root->childs[0]->value->members_count
				, root->childs[1]->ivalue, (root->childs[0]->value->members_count == 0 ? " (is the array correctly initialized ?)": ""));
				err_display(s);
			}
			else
			{
				root->value = root->childs[0]->value->members[root->childs[1]->ivalue];
				root->ref = &(root->childs[0]->value->members[root->childs[1]->ivalue]);
			}
			//fprintf(stderr, "%d\n", root->childs[0]->value_type->type);
		break;
		
		case NNA:
			//Check if index is of type integer
			syna_execute(root->childs[0]); //Type
			syna_execute(root->childs[1]); //Size
			if (err_check_type(root->childs[1]->value_type, syna_create_type(INT, NULL)))
			{
				root->value_type = syna_create_type(ARRAY, root->childs[0]->value_type);
				root->value = env_create_array(root->childs[1]->ivalue, root->childs[0]->value_type);
			}
		break;
		
		case NBRANCH:
			syna_execute(root->childs[0]);
			syna_execute(root->childs[1]);
		break;
		
		case NITE:
			//Check if condition is of type Boolean
			syna_execute(root->childs[0]);
			syna_check(root->childs[1]);
			syna_check(root->childs[2]);
			if (err_check_type(root->childs[0]->value_type, syna_create_type(BOOL, NULL)))
			{
				if (root->childs[0]->value->value > 0)
					syna_execute(root->childs[1]);
				else
					syna_execute(root->childs[2]);
			}
		break;
		
		case NWD:
			//Check if condition is of type Boolean
			syna_execute(root->childs[0]);
			syna_check(root->childs[1]);
			if (root->childs[0]->value != NULL && root->childs[0]->value->type != NULL && err_check_type(root->childs[0]->value_type, syna_create_type(BOOL, NULL)))
				while (root->childs[0]->value != NULL && root->childs[0]->value->value > 0)
				{
					syna_execute(root->childs[1]);
					syna_execute(root->childs[0]);
				}
			
		break;
		
		case NAAF:	//TODO: do you like my car ?
			//Check if value and destination are of same type
			syna_execute(root->childs[0]);
			syna_execute(root->childs[1]);
			if (err_check_type(root->childs[1]->value_type, root->childs[0]->value_type))
			{
				if (root->childs[1]->value_type->type != ARRAY)
					root->childs[0]->value->value = root->childs[1]->value->value;
				else
				{
					root->childs[0]->value->members = root->childs[1]->value->members;
					root->childs[0]->value->members_count = root->childs[1]->value->members_count;
				}
			}
		break;
		
		case NNVAR:
			//Check if value and destination are of same type

		break;
		
		case NVAF:
			//Check if value and destination are of same type
			syna_execute(root->childs[1]);
			syna_execute(root->childs[0]);
			if (err_check_type(root->childs[1]->value_type, root->childs[0]->value_type))
			{
				root->childs[0]->variable->value = root->childs[1]->value;
			}
			
			break;
		
		case NSKIP:
			//eh	
			break;
		
		case NEXPR:
			
		break;
		
		case NVDEF:
			root->childs[0]->variable = env_get_variable(root->childs[0]->string, var_declaration);
			if (root->childs[1] != NULL)
				syna_execute(root->childs[1]);
				
			root->childs[0]->variable->type = root->value_type;
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
			env_change_context(root->string, 0);
			syna_link_args_to_func(f_context, root->childs[0]);	//define args
		break;
		
		case NFDEF:
			syna_execute(root->childs[1]); //define ret type
			root->value_type = root->childs[1]->value_type;
			env_add_function(root->string, root->value_type, root->variable);
			env_change_context(root->string, 0);
			syna_link_args_to_func(f_context, root->childs[0]);	//define args
		break;
	
		case NPBODY:
		var_declaration = 1;
		syna_execute(root->childs[0]); //Procedure declaration
		//Context already changed
		syna_execute(root->childs[1]); //Add local variables
		//When called, must execute root->childs[2]
		var_declaration = 0;
		f_context->body = root->childs[2];
		syna_check(root->childs[2]);
		env_change_context("main_program", 0);
		break;
		
		case NFBODY:
		var_declaration = 1;
		syna_execute(root->childs[0]); //Function declaration
		//Context already changed
		syna_execute(root->childs[1]); //Add local variables
		env_add_variable(root->childs[0]->string, root->childs[0]->value_type); // Add return value
		//When called, must execute root->childs[2]
		var_declaration = 0;
		f_context->body = root->childs[2];
		syna_check(root->childs[2]);
		env_change_context("main_program", 0);
		break;
		
		case NFPCALL:
		{
			//syna_execute(root->childs[0]);
			pp_func f = env_get_function(root->string, 0);
			if (f != NULL)
			{
				root->value_type = f->ret_type;
				if (!err_check_arguments(root->childs[0], f, 0, env_get_args_number(f)))
				{
					char* s = (char*) malloc(sizeof(char) * ERR_BUFFER_SIZE);
					sprintf(s, "Too few arguments for %s", f->name);
					err_display(s);
				}else{
					env_change_context(f->name, 0);
					syna_execute(f->body);
					if (f->ret_type->type != NONE)
						root->variable = env_get_variable(f->name, 0);
						
					env_change_context("main_program", 0);
				}
			}else{
				root->value_type = syna_create_type(NONE, NULL);
			}
		}
		break;
	}
}

void syna_check(syna_node root)
{
	line_position = root->line_position;
	switch (root->type) {
		case NEMPTY:
			//eh
			break;
		
		case NROOT:
			var_declaration = 1;
			syna_check(root->childs[0]);
			var_declaration = 0;
			syna_check(root->childs[1]);
			syna_check(root->childs[2]);
			break;
		
		case NOPI:
			//Check if both values are of type Integer 
			syna_check(root->childs[0]);
			syna_check(root->childs[1]);
			err_check_type(root->childs[0]->value_type, syna_create_type(INT, NULL));
			err_check_type(root->childs[1]->value_type, syna_create_type(INT, NULL));
		break;
		
		case NOPB:
			//Check if both values are of type Boolean
			if (root->opb != NOT)
				syna_check(root->childs[0]);
			
			syna_check(root->childs[1]);
			int err_status = 1;
			switch (root->opb) {
				case OR:
				case AND:
					err_status = err_check_type(root->childs[0]->value_type, syna_create_type(BOOL, NULL));
				case NOT:
					err_status = err_check_type(root->childs[1]->value_type, syna_create_type(BOOL, NULL));			
				break;
				
				case LT:
				case EQ:
					err_status = err_check_type(root->childs[0]->value_type, syna_create_type(INT, NULL))
								* err_check_type(root->childs[1]->value_type, syna_create_type(INT, NULL));
				break;			
			}
		break;
		
		case NPBA:
		   syna_check(root->childs[0]);
		   root->value_type = root->childs[0]->value_type;
		   root->value = root->childs[0]->value;
		break;
		
		case NVALUE:
			//Check type ?
		break;
		
		case NVAR:
			{
			//Check type
			pp_var v = env_get_variable(root->string, var_declaration);
			root->value_type = v->type;	
			}
		break;
		
		case NARRAY:	//TODO: aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
			syna_check(root->childs[1]);//index
			err_check_type(root->childs[1]->value_type, syna_create_type(INT, NULL));
			syna_check(root->childs[0]);//array-side
			//root->value_type = syna_create_type(ARRAY, root->childs[0]->value_type);
			root->value_type = root->childs[0]->value_type->next;
			//fprintf(stderr, "%d\n", root->childs[0]->value_type->type);
		break;
		
		case NNA:
			//Check if index is of type integer
			syna_check(root->childs[0]);
			syna_check(root->childs[1]);
			err_check_type(root->childs[1]->value_type, syna_create_type(INT, NULL));
			root->value_type = syna_create_type(ARRAY, root->childs[0]->value_type);
		break;
		
		case NBRANCH:
			syna_check(root->childs[0]);
			syna_check(root->childs[1]);
		break;
		
		case NITE:
			//Check if condition is of type Boolean
			syna_check(root->childs[0]);
			err_check_type(root->childs[0]->value_type, syna_create_type(BOOL, NULL));
			syna_check(root->childs[1]);
			syna_check(root->childs[2]);
		break;
		
		case NWD:
			//Check if condition is of type Boolean
			syna_check(root->childs[0]);
			syna_check(root->childs[1]);
			err_check_type(root->childs[0]->value_type, syna_create_type(BOOL, NULL));
		break;
		
		case NAAF:	//TODO: do you like my car ?
			//Check if value and destination are of same type
			syna_check(root->childs[0]);
			syna_check(root->childs[1]);
			err_check_type(root->childs[1]->value_type, root->childs[0]->value_type);
		break;
		
		case NNVAR:
			//Check if value and destination are of same type

		break;
		
		case NVAF:
			//Check if value and destination are of same type
			syna_check(root->childs[1]);
			syna_check(root->childs[0]);
			err_check_type(root->childs[1]->value_type, root->childs[0]->value_type);
			break;
		
		case NSKIP:
			//eh	
			break;
		
		case NEXPR:
			
		break;
		
		case NVDEF:
			root->childs[0]->variable = env_get_variable(root->childs[0]->string, var_declaration);
			if (root->childs[1] != NULL)
				syna_check(root->childs[1]);
				
			root->childs[0]->variable->type = root->value_type;
			break;
		
		case NTYPE: 
			if (root->childs[0] != NULL)
			{
				syna_check(root->childs[0]);
				root->value_type->next = root->childs[0]->value_type;
			}
		break;
		
		case NFPCALL:
		{
			//syna_check(root->childs[0]);
			pp_func f = env_get_function(root->string, 0);
			if (f != NULL)
			{
				root->value_type = f->ret_type;
				if (!err_check_arguments(root->childs[0], f, 0, env_get_args_number(f)))
				{
					char* s = (char*) malloc(sizeof(char) * ERR_BUFFER_SIZE);
					sprintf(s, "Too few arguments for %s", f->name);
					err_display(s);
				}
			}else{
				root->value_type = syna_create_type(NONE, NULL);
			}
		}
		break;
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
			env_display_value(v_root->value, 1);
			v_root = v_root->next;
		}
		
		f = f->next;
	}
	
	printf("===== END =====\n");
}

void exe_stop()
{
	fprintf(stderr, "Execution stopped.\n");
	exit(-1);
}

void env_report()
{
	env_change_context("main_program", 0);
	env_display();
}