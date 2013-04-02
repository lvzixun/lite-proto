#include "lp.h"
#include "../lp_conf.h"
#include "lp_file.h"
#include <stdlib.h>
#include <stdio.h>

lp_env* get_lp_env()
{
	lp_env* ret = (lp_env*)malloc(sizeof(*ret));
	memset(ret, 0, sizeof(*ret));

	check_fail(get_lex_env(&ret->lex_envV), NULL);
	check_fail(get_parse_env(&ret->parse_envV, &ret->lex_envV.lex_list), NULL);
	return ret;
}

int clr_lp_env(lp_env* lp)
{
	check_null(lp, LP_FAIL);
	cls_lex_env(&lp->lex_envV);
	free_parse_env(&lp->parse_envV);
	get_parse_env(&lp->parse_envV, &lp->lex_envV.lex_list);
	return LP_TRUE;
}

void free_lp_env(lp_env* lp)
{
	if(lp==NULL)
		return;
	free_lex_env(&lp->lex_envV);
	free_parse_env(&lp->parse_envV);
	free(lp);
}

int lp_inter(lp_env* lp, char* name)
{
	FILE* fp = NULL;
	slice sp = {0};
	lp_string t_name;
	memset(&t_name, 0, sizeof(t_name));

	check_null(lp, (print("[Serious error]: not lp_env!\n"), LP_FAIL));
	t_name =  lp_string_new(name);
	if(read_file(name, &sp)==LP_FAIL)
		goto FAIL_END;
	if(lp_lex(&lp->lex_envV, &sp)==LP_FAIL)
		goto FAIL_END;
	if(lp_parse(&lp->parse_envV)==LP_FAIL)
		goto FAIL_END;

	lp_string_cats(&t_name, ".lpb");
	fp = fopen((char*)t_name.str.list_p, "wb");
	if(fp)
	{
		fwrite(lp->parse_envV.parse_out.list_p, lp->parse_envV.parse_out.list_len, 1, fp);
		fclose(fp);
		print("inter success[file: %s]\n", (char*)t_name.str.list_p);
		goto SUCCESS_END;
	}
	
FAIL_END:
	print("inter fail<file: %s>!\n", ((char*)t_name.str.list_p)?((char*)t_name.str.list_p):("null") );
SUCCESS_END:
	clr_lp_env(lp);
	lp_string_free(&t_name);
	if(sp.b_sp)
		free(sp.b_sp);

	return LP_TRUE;
}

void lp_arg(lp_env* lp, char* args[])
{
	if(args[1]==0 || strcmp(args[1], "-h")==0)
	{
		print("%s\n%s%s\n%s\n%s\n", 
			  "lp    help:",
			  "-v :  ", LP_VERSION,
			  "-o :  parse the .mes file into .mes.lpb data file. exp: lp mesfile[ mesfile[ mesfile[ ...]]]",
			  "-h :  lp help, by zixunlv."
			 );
	}
	else if(strcmp(args[1], "-v")==0)
	{
		print("%s\n", LP_VERSION);
	}
	else if(strcmp(args[1], "-o")==0)
	{
		int i;
		for(i=2; args[i]!=NULL; i++){
			if(lp_inter(lp, args[i])==LP_FAIL)
				return;
		}
	}
	else
		print("%s cmd is error! please input %s -h  see help.\n", args[0], args[0]);
}


int main(int argc, char* args[])
{
	lp_env* lp = get_lp_env();

#ifndef _DEBUG
	lp_arg(lp, args);
#else
	lp_inter(lp, "./testlex.mes");
#endif
	free_lp_env(lp);
//	print_mem();
	return 0;
}

