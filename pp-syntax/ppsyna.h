enum e_pp_type_id { NONE, INT, BOOL, ARRAY };
enum e_syna_opi { INONE, PL, MO, MU };
enum e_syna_opb { BNONE, OR, LT, EQ, AND, NOT};
enum e_syna_node_id { NEMPTY, NOPI, NOPB, NPBA, NVALUE, NVAR, NARRAY, NBRANCH, NITE, NWD, NAAF, NVAF, NSKIP, NEXPR, NADEF, NTYPE, NPDEF, NFDEF}

typedef enum e_pp_type_id pp_type_id;
typedef enum e_syna_opi syna_opi;
typedef enum e_syna_opb syna_opb;
typedef enum e_syna_node_id syna_node_id;

struct s_pp_type{
	pp_type_id type;
	struct s_pp_type* next_dim;
};

typedef struct s_pp_type* pp_type;

struct s_pp_var{
	char* name;
	pp_type type;
	struct s_pp_var* next;
};

typedef struct s_pp_var* pp_var;

struct s_pp_func{
	char* name;
	pp_type ret_type;
	pp_var context;
	pp_var context_current;
	pp_var args;		//reverse order
	struct s_pp_func* next;
};

typedef struct s_pp_func* pp_func;

struct s_syna_node{
	syna_node_id type;
	struct s_syna_node** childs;
	
	//any of these may be useless
	int value;
	pp_type value_type;
	pp_var variable;
	pp_func function;
	syna_opi opi;
	syna_opb = opb;
};


void env_initialize();
void env_add_variable(char* name, pp_type type);
void env_add_function(char* name, pp_type ret_type, pp_var args);
void env_add_lcl_variable(pp_var* lcl_root, char* name, pp_type type);
pp_type env_get_type_of_variable(char* name);
pp_type env_get_type_of_function(char* name);
void env_change_scope(char* scope);

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
syna_node syna_adef_node(syna_node dest, syna_node value);
syna_node syna_type_node(pp_type_id type, syna_node next);
syna_node syna_pdef_node(syna_node name, syna_node args);
syna_node syna_fdef_node(syna_node name, syna_node args, syna_node ret);
void env_display();
