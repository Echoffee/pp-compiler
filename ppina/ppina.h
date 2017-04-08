enum e_pp_type_id { NONE, INT, BOOL, ARRAY, RET};
enum e_syna_opi { INONE, PL, MO, MU };
enum e_syna_opb { BNONE, OR, LT, EQ, AND, NOT};
//                       0     1     2     3     4     5       6     7        8        9      10    11   12   13   14        15    16     17     18      19     20     21       22      23
enum e_syna_node_id { NEMPTY, NROOT, NOPI, NOPB, NPBA, NVALUE, NVAR, NNVAR, NARRAY, NBRANCH, NITE, NWD, NAAF, NVAF, NSKIP, NEXPR, NVDEF, NTYPE, NPDEF, NFDEF, NPBODY, NFBODY, NFPCALL, NNA};
enum e_pp_var_scope { GLOBAL, LOCAL};
typedef enum e_pp_type_id pp_type_id;
typedef enum e_syna_opi syna_opi;
typedef enum e_syna_opb syna_opb;
typedef enum e_syna_node_id syna_node_id;
typedef enum e_pp_var_scope pp_var_scope;
typedef struct s_syna_node* syna_node;

struct s_pp_type{
	pp_type_id type;
	struct s_pp_type* next;
};

typedef struct s_pp_type* pp_type;

struct s_pp_value{
	pp_type type;
	int value;
	int members_count;
	struct s_pp_value** members;
	struct s_pp_value* next;
};

typedef struct s_pp_value* pp_value;

struct s_pp_var{
	char* name;
	pp_type type;
	pp_value value;
	pp_var_scope scope;
	struct s_pp_var* next;
};

typedef struct s_pp_var* pp_var;

struct s_pp_func{
	char* name;
	pp_type ret_type;
	pp_var context;
	pp_var context_current;
	pp_var args;		//reverse order
	pp_var args_current;
	syna_node body;
	pp_value value;
	struct s_pp_func* next;
};

typedef struct s_pp_func* pp_func;

struct s_syna_node{
	syna_node_id type;
	struct s_syna_node** childs;
	
	//any of these may be useless
	char* string;
	int line_position;
	pp_var variable;
	pp_func function;
	pp_value value;
	syna_opi opi;
	syna_opb opb;
};

struct s_pp_context{
	pp_var context;
	pp_var current_context;
	pp_value return_value;
	char* context_name;
};

typedef struct s_pp_context* pp_context;

void incr_line();
void env_initialize();
pp_var env_add_variable(char* name, pp_type type);
pp_func env_add_function(char* name, pp_type ret_type, pp_var args);
pp_var env_add_lcl_variable(pp_var lcl_parent, char* name, pp_type type);
pp_type env_get_type_of_variable(char* name, int debug);
pp_type env_get_type_of_function(char* name, int debug);
void env_change_scope(char* scope);
void env_change_context(char* context, int debug);

syna_node syna_opi_node(syna_node member_left, syna_node member_right, syna_opi op);
syna_node syna_opb_node(syna_node member_left, syna_node member_right, syna_opb op);
syna_node syna_p_node(syna_node content);
syna_node syna_int_node(int value);
syna_node syna_var_node(char* name);
syna_node syna_bool_node(int value);
syna_node syna_array_node(syna_node member, syna_node index);
syna_node syna_branch_node(syna_node left, syna_node right);
syna_node syna_ITE_node(syna_node cond, syna_node th, syna_node el);
syna_node syna_WD_node(syna_node cond, syna_node d);
syna_node syna_aaf_node(syna_node dest, syna_node value);
syna_node syna_vaf_node(syna_node dest, syna_node value);
syna_node syna_skip_node();
syna_node syna_a_node(syna_node content);
syna_node syna_empty_node();
syna_node syna_expr_node(syna_node expr);
syna_node syna_vdef_node(syna_node dest, syna_node type);
syna_node syna_type_node(pp_type_id type, syna_node next);
syna_node syna_pdef_node(char* name, syna_node args);
syna_node syna_fdef_node(char* name, syna_node args, syna_node ret);
syna_node syna_root_node(syna_node def_global_vars, syna_node def_pf, syna_node c);
syna_node syna_new_var_node(char* name);
syna_node syna_pbody_node(syna_node def, syna_node def_vars, syna_node body); //body of func/proc
syna_node syna_fbody_node(syna_node def, syna_node def_vars, syna_node body);
syna_node syna_call_func_node(char* name, syna_node args);
syna_node syna_newarray_node(syna_node type, syna_node expr);

void syna_execute(syna_node root, pp_context context);
void ina_execute(pp_func f, syna_node args);
void syna_check(syna_node root, pp_context context);
void err_display();
void err_report();
void env_report();
void env_display(pp_context context);

void exe_stop();
pp_var exe_get_variable(char* name, pp_context context);
pp_context exe_create_context();