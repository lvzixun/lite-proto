#include "lib_al.h"


int lib_array_new(llp_array* al_out)
{
	check_null(al_out, LP_FAIL);
	memset(al_out, 0, sizeof(*al_out));
	al_out->vp = al_out->vt;

	return LP_TRUE;
}

int lib_array_add(llp_array* al, llp_value* llp_v)
{
	check_null(al, LP_FAIL);
	check_null(llp_v, LP_FAIL);
	if(al->lens >= 1)
	{
		if(al->lens==1 && al->vp==al->vt )
		{
			al->vp = (llp_value*)malloc(sizeof(llp_value)*EXT_AL_LENS);
			memset(al->vp, 0, sizeof(llp_value)*EXT_AL_LENS);
			(al->vp)[0] = al->vt[0];
		}
		else
		{
			if( (al->lens+1)%EXT_AL_LENS == 0)
				al->vp = (llp_value*)realloc(al->vp, sizeof(llp_value)*((al->lens+1)+EXT_AL_LENS));
		}
	}
	
	al->vp[al->lens++]=*llp_v;
	return LP_TRUE;
}


llp_value* lib_array_inx(llp_array* al, unsigned int inx)
{
	check_null(al, NULL);
	if(al->lens<=inx)
		return NULL;

	return &(al->vp[inx]);
}

void lib_array_clr(llp_array* al, al_free free_func)
{
	size_t i=0;
	if(al==NULL)
		return;

	for(i=0; i<al->lens && free_func; i++)
		free_func(al->vp+i);

	al->lens =0;
}

void lib_array_free(llp_array* al, al_free free_func)
{
	if(al==NULL)
		return;
	lib_array_clr(al, free_func);
	if(al->vp != al->vt)
		free(al->vp);
	memset(al, 0, sizeof(*al));
}
