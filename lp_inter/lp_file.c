#include "lp.h"
#include "../lp_conf.h"
#include "lp_file.h"

int read_file(char* file_name, slice* sp)
{
	int ret = LP_TRUE;
	FILE* fp = NULL;
	long fs = 0;
	
	check_null(file_name, (print("parse file not exist!\n"), LP_FAIL));
	fp = fopen(file_name, "rb");
	fs = f_size(fp);
	check_null(fp, (print("read file: %s is error!\n", file_name), LP_FAIL));
	sp->sp_size = (size_t)fs + 2;
	sp->sp = (byte*)malloc(sp->sp_size);
	memset(sp->sp, 0, sp->sp_size);
	sp->b_sp = sp->sp;
	
	if(fread(sp->sp, sizeof(char), sp->sp_size, fp) != sp->sp_size*sizeof(char))
		ret = LP_FAIL;
	fclose(fp);
	return ret;
}

