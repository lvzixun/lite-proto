#ifndef  _LP_H_
#define  _LP_H_
#include "lp_lex.h"
#include "lp_parse.h"

#define LP_VERSION  "lite-proto interpreter version:2.0"

typedef struct _lp_env{
	lp_lex_env lex_envV;
	lp_parse_env parse_envV;	
}lp_env;

int lp_inter(lp_env* lp, char* name);

#endif

