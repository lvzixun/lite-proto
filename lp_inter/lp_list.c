#include "lp_list.h"
#include "../lp_conf.h"

int _lp_list_new(lp_list* lp_l, size_t one_size, size_t def_size, list_Fnew new_func, list_Ffree free_func)
{
	check_null(lp_l, LP_FAIL);
	lp_l->one_size = one_size;
	lp_l->list_len = 0;
	lp_l->list_def_size = def_size;
	lp_l->list_p = (byte*)malloc(one_size*lp_l->list_def_size);
	memset(lp_l->list_p, 0, one_size*lp_l->list_def_size);
	lp_l->list_size = lp_l->list_def_size;
	lp_l->new_func = new_func;
	lp_l->free_func = free_func;
	
	return LP_TRUE;
}

int lp_list_relloc(lp_list* lp_l)
{
	check_null(lp_l, LP_FAIL);
	check_null(lp_l->list_p, LP_FAIL);
	
	if(lp_l->list_len < lp_l->list_size)
		return LP_NIL;
	
	check_null(lp_l->list_p=realloc(lp_l->list_p, (lp_l->list_size+=lp_l->list_def_size)*lp_l->one_size), LP_FAIL);
	return LP_TRUE;
}

int lp_list_add(lp_list* lp_l, byte* data_p)
{
	check_null(lp_l, LP_FAIL);
	check_null(lp_l->list_p, LP_FAIL);
	check_null(data_p, LP_FAIL);
	
	check_fail(lp_list_relloc(lp_l), LP_FAIL);
	if(lp_l->new_func)
		lp_l->new_func(data_p);
	memcpy((lp_l->list_p + lp_l->one_size*lp_l->list_len), data_p, lp_l->one_size);
	(lp_l->list_len)++;
	
	return LP_TRUE;
}

void* lp_list_inx(lp_list* lp_l, size_t inx)
{
	check_null(lp_l, NULL);
	check_null(lp_l->list_p, NULL);
	
	return (inx>=lp_l->list_len)?(NULL):( (void*)(&lp_l->list_p[lp_l->one_size*inx]) );
}


int lp_list_free(lp_list* lp_l)
{
	size_t i=0;
	check_null(lp_l, LP_FAIL);
	check_null(lp_l->list_p, LP_NIL);
	
	for(i=0; i<lp_l->list_len; i++)
	{
		if(lp_l->free_func)
			lp_l->free_func(lp_list_inx(lp_l, i));
	}
	free(lp_l->list_p);
	memset(lp_l, 0, sizeof(lp_list));
	return LP_TRUE;
}
