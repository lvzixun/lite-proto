#include "lp_lex.h"
#include "../lp_conf.h"
#include "lp_table.h"

// token typer to string
char* ts[] = {
	"error char",
	"number",
	"{",
	"}",
	"=",
	"[",
	"]",
	"identifier",
	";",
	".",
	",",
	"extern",

	"integer",
	"real",
	"string",
	"bytes",
	"message",

	"null"
};


//	key
 static lp_key lp_sk[] = {
	 {"message", t_Kmessage}, {"integer", t_Kinteger}, {"string", t_kstring},
	 {"real", t_Kreal}, {"bytes", t_Kbytes}, {"extern", t_Kextern}, 
	{NULL, t_error}
};

#define now_char(p)					( (p)&&((p)->sp)&&(*((p)->sp)) )?( *((char*)((p)->sp)) ):(0)
#define _next_char(p)				( (p)&&((p)->sp)&&(*((p)->sp)) )?( *((char*)((p)->sp))++ ):(0)
#define char_type(p, c)				( (p)->char_enum[(int)(c)] )
//#define char_type(p, c)			( (((int)c)>=0 || ((int)c)<128)?((p)->char_enum[(int)(c)]):(l_null) )

static int lp_lex_char(lp_lex_env* env_p, slice* buff);
static int lp_lex_number(lp_lex_env* env_p, slice* buff);


char next_char(slice* p)
{
	char ret = 0;
	if(p&&p->sp&&*(p->sp))
	{
		ret = *((char*)(p->sp));
		(p->sp)++;
	}

	return ret;
}

static lp_token lp_new_token(lp_lex_env* env_p, byte tt, lp_string name)
{
	lp_token lp_ret = {0};
	lp_ret.line = env_p->line;
	lp_ret.name = name;
	lp_ret.type = tt;

	return lp_ret;
}

static void lp_add_token(lp_lex_env* env_p, lp_token lp_t)
{
	lp_list_add( &((env_p)->lex_list), (byte*)(&(lp_t)) );
}

static byte lp_look_key(lp_lex_env* env_p, char* str)
{
	int inx = 0;
	check_null(env_p, t_error);
	check_null(str, t_error);
	inx = _BKDRHash(str, LEX_KEY_MAX);
	
	return (env_p->lp_k[inx].s_tt==t_error || strcmp(env_p->lp_k[inx].s_key, str))?(t_error):(env_p->lp_k[inx].s_tt);
}

int lp_lex_token_free(lp_token* tp)
{
	check_null(tp, LP_FAIL);
	lp_list_free(&tp->name.str);

	return LP_TRUE;
}

void free_lex_env(lp_lex_env* le)
{
	if(le==NULL)
		return;

	lp_list_free(&le->lex_list);
	memset(le, 0, sizeof(*le));
}

int cls_lex_env(lp_lex_env* le)
{
	check_null(le, LP_FAIL);
	lp_list_free(&le->lex_list);
	le->line = 1;
	check_fail(lp_list_new(&le->lex_list, sizeof(lp_token), NULL, (list_Ffree)lp_lex_token_free), LP_FAIL);

	return LP_TRUE;
}

int get_lex_env(lp_lex_env* le)
{
	int i=0;
	
	// init key
	check_null(le, LP_FAIL);
	for(i=0; lp_sk[i].s_key; i++)
	{
		int inx = _BKDRHash(lp_sk[i].s_key, LEX_KEY_MAX);
		if(le->lp_k[inx].s_key)
		{
			print("[init error]lp_key is not null! key=\"%s\" \n", le->lp_k[inx].s_key);
			return LP_FAIL;
		}
		else
			le->lp_k[inx] = lp_sk[i];
	}
	memset(le->char_enum, 0, sizeof(le->char_enum));
	le->char_enum['\t']= l_skip;
	le->char_enum[' ']= l_skip;
	le->char_enum['\r'] = l_skip;
	le->char_enum['\n']= l_n;
	le->char_enum['_']= l_char;
	le->char_enum['{']= l_lb;
	le->char_enum['}']= l_rb;
	le->char_enum['=']= l_ass;
	le->char_enum['[']= l_ll;
	le->char_enum[']']= l_rl;
	le->char_enum[',']= l_ca;
	le->char_enum['#']= l_text;
	le->char_enum[';']= l_end;
	le->char_enum['.']= l_clo;
	for(i='a'; i<='z'; i++)
		le->char_enum[i] = l_char;
	for(i='A'; i<='Z'; i++)
		le->char_enum[i] = l_char;
	for(i='0'; i<='9'; i++)
		le->char_enum[i] = l_num;
	
	le->line = 1;
	check_fail(lp_list_new(&le->lex_list, sizeof(lp_token), NULL, (list_Ffree)lp_lex_token_free), LP_FAIL);
	return LP_TRUE;
}


int lp_lex(lp_lex_env* env_p, slice* buff)
{
	char at_char = 0;
	check_null(env_p, LP_FAIL);
	check_null(buff, LP_FAIL);
	for(; (at_char=now_char(buff))!=0 ; )
	{
		switch(char_type(env_p, at_char))
		{
		case l_char:
			lp_lex_char(env_p, buff);
			break;
		case l_num:
			lp_lex_number(env_p, buff);
			break;
		case l_end:
			lp_add_token(env_p, lp_new_token(env_p, t_end, lp_string_new(NULL)));
			next_char(buff);
			break;
		case l_clo:
			{
				lp_add_token(env_p, lp_new_token(env_p, t_clo, lp_string_new(NULL)));
				next_char(buff);
				at_char=now_char(buff);
				if(char_type(env_p, at_char) != l_char)
				{
					print("lex[error line: %d] token \".\" must be followed identifier!\n", env_p->line);
					return LP_FAIL;
				}
			}
			break;
		case l_ass:
			lp_add_token(env_p, lp_new_token(env_p, t_ass, lp_string_new(NULL)));
			next_char(buff);
			break;
		case l_lb:
			lp_add_token(env_p, lp_new_token(env_p, t_lb, lp_string_new(NULL)));
			next_char(buff);
			break;
		case l_ll:
			lp_add_token(env_p, lp_new_token(env_p, t_ll, lp_string_new(NULL)));
			next_char(buff);
			break;
		case l_rb:
			lp_add_token(env_p, lp_new_token(env_p, t_rb, lp_string_new(NULL)));
			next_char(buff);
			break;
		case l_ca:
			lp_add_token(env_p, lp_new_token(env_p, t_ca, lp_string_new(NULL)));
			next_char(buff);
			break;
		case l_rl:
			lp_add_token(env_p, lp_new_token(env_p, t_rl, lp_string_new(NULL)));
			next_char(buff);
			break;
		case l_text:
			{
				char lc = now_char(buff);
				for(; lc && lc!='\n'; )
				{
					next_char(buff);
					lc = now_char(buff);
				}
			}
			break;
		case  l_n:
			(env_p->line)++;
		case  l_skip:
			next_char(buff);
			break;
		default:
			print("lex[error line: %d] find can not lex char!\n", env_p->line);
			return LP_FAIL;
		}
	}

	return LP_TRUE;
}


static int lp_lex_char(lp_lex_env* env_p, slice* buff)
{
	char at_char = 0;
	byte t_t;
	lp_string name = lp_string_new("");
	for(;	(at_char=now_char(buff)) && 
			(char_type(env_p, at_char)==l_char || char_type(env_p, at_char)==l_num) ;
		next_char(buff)
	   )
	{
		lp_string_cat(&name, at_char);
	}
	
	t_t = lp_look_key(env_p, (char*)name.str.list_p);
	if(t_t)
	{
		lp_string_free(&name);
		lp_add_token(env_p, lp_new_token(env_p, t_t, lp_string_new(NULL)));
	}
	else
	{
		lp_add_token(env_p, lp_new_token(env_p, t_ide, name));
	}

	return LP_TRUE;
}

static int lp_lex_number(lp_lex_env* env_p, slice* buff)
{
	char at_char = 0;
	lp_string name = lp_string_new("");
	for(;  (at_char=now_char(buff)) && (char_type(env_p, at_char)==l_num); next_char(buff))
		lp_string_cat(&name, at_char);

	lp_add_token(env_p, lp_new_token(env_p, t_num, name));
	return LP_TRUE;
}

/*
int lp_lex_print(lp_lex_env* env_p)
{
	
	size_t i=0;
	if(env_p==NULL || env_p->lex_list.list_p==NULL)
		return LP_FAIL;
	
	print("-----print token-----\n");
	for(i=0; i<env_p->lex_list.list_len; i++)
	{
		lp_token* lp_tp = (lp_token*)lp_list_inx(&env_p->lex_list, i);
		check_null(lp_tp, LP_FAIL);
		
		print(	"line:%d type:%s name:%s\n", 
				lp_tp->line, ts[lp_tp->type], 
				(lp_tp->name.str.list_p)?((char*)(lp_tp->name.str.list_p)):("<not>") 
			);
	}

	return LP_TRUE;
}
*/

