#ifndef  _LP_PARSE_H_
#define  _LP_PARSE_H_
#include "../lp_conf.h"
#include "lp_table.h"

enum {
	e_req,
	e_rep
};

typedef struct _lp_parse_env{
	lp_list		parse_out;
	lp_table	parse_table;
	lp_list*	token_list;
	size_t		read_inx;
	unsigned int	line;
	
	lp_list		clo_stack;	
	lp_string   mes_name;
}lp_parse_env;

int get_parse_env(lp_parse_env* lp_p, lp_list* token_list);
int free_parse_env(lp_parse_env* lp_p);
int lp_parse(lp_parse_env* lp_p);

#endif

