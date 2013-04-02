#ifndef  _LP_LEX_H
#define  _LP_LEX_H
#include "../lp_conf.h"
#include "lp_table.h"

extern char* ts[];

// char type
typedef enum _e_c{
	l_null,		// null
	l_char,		// a-z, A-Z, _
	l_num,		// 0-9
	l_lb,		// {
	l_rb,		// }
	l_ass,		// =
	l_ll,		// [
	l_rl,		// ]
	l_n,		// \n
	l_text,		// #
	l_end,		// ;
	l_clo,		// .
	l_skip,		// space, \r, \t
	l_ca		// ,
}e_c;

// token type
typedef enum _e_t{
	t_error,		// null
	t_num,			// num
	t_lb,			// {
	t_rb,			// }
	t_ass,			// =
	t_ll,			// [
	t_rl,			// ]
	t_ide,			// IDE
	t_end,			// ;
	t_clo,			// .
	t_ca,			// ,
	t_Kextern,		// extern

	t_Kinteger,		// integer
	t_Kreal,		// real
	t_kstring,		// string
	t_Kbytes,		// bytes
	t_Kmessage,		// message
	
	t_count
}e_t;


// token type to llp type
#define tt2lt(tt)	((tt)-t_Kinteger)


typedef struct _lp_key{   
	char* s_key;
	byte s_tt;
}lp_key;

#define LEX_KEY_MAX 32
typedef struct _lp_lex_env{
	lp_key lp_k[LEX_KEY_MAX];
	byte char_enum[128];
	lp_list lex_list;
	unsigned int line;
}lp_lex_env;

typedef struct _lp_token{
	unsigned int line;
	e_t type;
	lp_string name;
}lp_token;

#define watchs(t)	( (((byte)(t))>=t_count)?(ts[0]):(ts[(t)]) )
int get_lex_env(lp_lex_env* le);
int cls_lex_env(lp_lex_env* le);
void free_lex_env(lp_lex_env* le);
int lp_lex(lp_lex_env* env_p, slice* buff);


int lp_lex_print(lp_lex_env* env_p);


#endif


