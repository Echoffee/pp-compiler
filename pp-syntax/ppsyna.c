#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppsyna.h"

pp_var v_root = NULL;
pp_var v_current = NULL;

pp_func f_root = NULL;
pp_func f_current = NULL;

void env_add_variable(char* name, pp_type type)
{
	pp_var v = (pp_var) malloc(sizeof(struct s_pp_var));
	v->name = strdup(name);
	v->type = type;
	v->next = NULL;
	
	if (v_current != NULL)
		v_current->next = v;
		
	if (v_root == NULL)
		v_root = v;
		
	v_current = v;
}

void env_add_function(char* name, pp_type ret_type, pp_var args)
{
	pp_func f = (pp_func) malloc(sizeof(struct s_pp_func));
	f->name = strdup(name);
	f->ret_type = type;
	f->args = args;
	
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
	while(v_root != NULL)
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
		
		printf(".\n");
		v_root = v_root->next;
	}
	
	while (f_root != NULL)
	{
		char s[10];
		switch (f_current->ret_type) {
			case NONE:
				sprintf(s, "Procedure");
				break;
				
			default:
				sprintf(s, "Function");
				break;
		}
		
		printf("%s %s (", s, f_root->name);
		display_args(f_root->args, 0);
		if (f_root->ret_type != NONE)
		{
			printf(" :");
			pp_type t_current = f_root->type;
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
		
		printf(".\n");
		f_root = f_root->next;
	}
	
}