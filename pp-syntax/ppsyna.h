enum e_pp_type_id { NONE, INT, BOOL, ARRAY };

typedef enum e_pp_type_id pp_type_id;

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
	int num_args;
	pp_var args;		//reverse order
	struct s_pp_func* next;
};

typedef struct s_pp_func* pp_func;



void env_add_variable(char* name, pp_type type);
void env_add_function(char* name, pp_type ret_type, pp_var args);
void env_add_lcl_variable(pp_var* lcl_root, char* name, pp_type type);

void env_display();
