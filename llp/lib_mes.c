#include "../llp.h"
#include "lib_mes.h"
#include "lp_conf.h"
#include "lib_lp.h"
#include "lib_table.h"
#include "lib_io.h"

static void llp_message_freeV(llp_value* lp_v);
static void llp_string_freeV(llp_value* lp_v);
static void llp_bytes_freeV(llp_value* lp_v);
char* malloc_string(char* str);
slice* malloc_slice(slice* sl);


static llp_mes* _llp_message_new(t_def_mes* def_mesP)
{
	llp_uint32 i=0;
	llp_mes* out_mes = NULL;
	check_null(def_mesP, NULL);
	check_null(def_mesP->message_field, NULL);
	out_mes = (llp_mes*)malloc(sizeof(llp_mes));
	memset(out_mes, 0, sizeof(llp_mes));
	
	out_mes->d_mes = def_mesP;
	out_mes->field_lens = def_mesP->message_count;
	out_mes->field_al = (llp_array*)malloc(sizeof(llp_array)*out_mes->field_lens);
	for(i=0; i<out_mes->field_lens; i++)
		lib_array_new(&out_mes->field_al[i]);
	
	return out_mes;
}

// create a  message object
LLP_API llp_mes*  llp_message_new(llp_env* env, char* mes_name)
{
	t_def_mes* mes_p= NULL;
	check_null(env, NULL);
	check_null(mes_name, NULL);

	check_null(mes_p=lib_Mmap_find(env->dmes, mes_name), NULL);
	return _llp_message_new(mes_p);
}

// get a message name 
LLP_API char* llp_message_name(llp_mes* lm)
{
	check_null(lm, NULL);
	return lm->d_mes->message_name;
}


typedef void (*lib_array_cf_func)(llp_array* al, al_free free_func);
static int _llp_message_cf(llp_mes* in_mes, lib_array_cf_func cf_func)
{
	size_t i=0;
	if(in_mes == NULL)
		return LP_FAIL;
	
	for(i=0; i<in_mes->field_lens; i++)
	{
		switch(tag_type(in_mes->d_mes->message_tfl[i].tag))
		{
		case LLPT_MESSAGE:
			cf_func(&in_mes->field_al[i], llp_message_freeV);
			break;
		case LLPT_STRING:
			cf_func(&in_mes->field_al[i], llp_string_freeV);
			break;
		case LLPT_BYTES:
			cf_func(&in_mes->field_al[i], llp_bytes_freeV);
			break;
		default:
			cf_func(&in_mes->field_al[i], NULL);
			break;
		}
	}

	return LP_TRUE;
}

LLP_API void llp_message_clr(llp_mes* in_mes)
{
	if(_llp_message_cf(in_mes, lib_array_clr)==LP_FAIL)
		return;
	llp_out_clr(&in_mes->sio);
}

LLP_API void  llp_message_free(llp_mes* in_mes)
{
	if(_llp_message_cf(in_mes, lib_array_free)==LP_FAIL)
		return;
	free(in_mes->field_al);
	llp_out_close(&in_mes->sio);
	free(in_mes);
}

static void llp_message_freeV(llp_value* lp_v)
{
	llp_message_free(lp_v->lp_mes);
}

static void llp_string_freeV(llp_value* lp_v)
{
	if(lp_v->lp_str)
		free(lp_v->lp_str);
}

static void llp_bytes_freeV(llp_value* lp_v)
{
	if(lp_v->lp_bytes){
		free(lp_v->lp_bytes->b_sp);
		free(lp_v->lp_bytes);
	}
}

int _llp_Wmes(llp_mes* lm, int inx, byte v_type, void* msd)
{
	llp_value lpv = {0};
	byte tag = 0;
	tag = lm->d_mes->message_tfl[inx].tag;
	
	// error : if a field is not repeated, so double add is error
	if( (tag_type(tag)!=v_type) || (tag_state(tag)==lpf_req &&  lm->field_al[inx].lens>=1) )
		return LP_FAIL;
	
	switch(tag_type(tag))
	{
	case LLPT_INTEGER:
		lpv.lp_integer = *((llp_integer*)msd);
		break;
	case LLPT_REAL:
		lpv.lp_real = *((llp_real*)msd);
		break;
	case LLPT_STRING:
		lpv.lp_str = malloc_string((char*)msd);
		break;
	case LLPT_BYTES:
		lpv.lp_bytes = malloc_slice((slice*)msd);
		break;
	case LLPT_MESSAGE:
		{
			lpv.lp_mes = _llp_message_new(lm->d_mes->message_tfl[inx].tms);
			*((llp_mes**)msd) = lpv.lp_mes;
		}
		break;
	default:
		return LP_FAIL;
	}

	lib_array_add(&lm->field_al[inx], &lpv);	
	return LP_TRUE;
}

// write a int at a message obj
static int llp_Wmes(llp_mes* lm, char* field_str, byte v_type, void* msd)
{
	int* id_p = NULL;

	check_null(lm, LP_FAIL);
	check_null(field_str, LP_FAIL);
	check_null(msd, LP_FAIL);
	check_null(id_p=lib_Fmap_find(lm->d_mes->message_field, field_str), LP_FAIL);

	return _llp_Wmes(lm, *id_p, v_type, msd);
}

LLP_API int llp_Wmes_bytes(llp_mes* lm, char* field_name, slice* sl)
{	
	check_fail(llp_Wmes(lm, field_name, LLPT_BYTES, (void*)sl), LP_FAIL);
	return LP_TRUE;
}

LLP_API int llp_Wmes_integer(llp_mes* lm, char* field_name, llp_integer number)
{
	check_fail(llp_Wmes(lm, field_name, LLPT_INTEGER, (void*)(&number)), LP_FAIL);
	return LP_TRUE;
}

LLP_API int llp_Wmes_real(llp_mes* lm, char* field_name, llp_real number)
{
	check_fail(llp_Wmes(lm, field_name, LLPT_REAL, (void*)(&number)), LP_FAIL);
	return LP_TRUE;
}

LLP_API int llp_Wmes_string(llp_mes* lm, char* field_name, char* str)
{
	check_fail(llp_Wmes(lm, field_name, LLPT_STRING, (void*)str), LP_FAIL);
	return LP_TRUE;
}

LLP_API llp_mes* llp_Wmes_message(llp_mes* lm, char* field_name)
{
	llp_mes* lms = NULL;
	check_fail(llp_Wmes(lm, field_name, LLPT_MESSAGE, (void*)(&lms)), NULL);
	return lms;
}

static llp_value* llp_Rmes(llp_mes* lm, char* field_str, byte v_type, unsigned int al_inx)
{
	int inx = 0;
	int* id_p = NULL;
	byte tag = 0;

	check_null(lm, NULL);
	check_null(field_str, NULL);
	check_null(id_p=lib_Fmap_find(lm->d_mes->message_field, field_str), NULL);
	inx = *id_p;
	tag = lm->d_mes->message_tfl[inx].tag;

	if(tag_type(tag)!=v_type)
		return NULL;

	switch(tag_type(tag))
	{
	case LLPT_INTEGER:
	case LLPT_REAL:
	case LLPT_STRING:
	case LLPT_MESSAGE:
	case LLPT_BYTES:
		break;
	default:
		return NULL;
	}

	return lib_array_inx(&lm->field_al[inx], al_inx);
}

LLP_API slice* llp_Rmes_bytes(llp_mes* lm, char* field_name, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, field_name, LLPT_BYTES, al_inx), NULL);
	
	return lpv->lp_bytes;
}

LLP_API llp_integer llp_Rmes_integer(llp_mes* lm, char* field_name, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, field_name, LLPT_INTEGER, al_inx), (llp_integer)0);
	
	return lpv->lp_integer;
}

LLP_API llp_real llp_Rmes_real(llp_mes* lm, char* field_name, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, field_name, LLPT_REAL, al_inx), (llp_real)0.0);
	
	return lpv->lp_real;
}

LLP_API char* llp_Rmes_string(llp_mes* lm, char* field_name, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, field_name, LLPT_STRING, al_inx), NULL);
	
	return lpv->lp_str;
}

LLP_API llp_mes* llp_Rmes_message(llp_mes* lm, char* field_name, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, field_name, LLPT_MESSAGE, al_inx), NULL);
	
	return lpv->lp_mes;
}

LLP_API llp_uint32 llp_Rmes_size(llp_mes* lm, char* field_name)
{
	int* id_p = NULL;

	check_null(lm, 0);
	check_null(field_name, 0);
	check_null(id_p=lib_Fmap_find(lm->d_mes->message_field, field_name), 0);

	return lm->field_al[*id_p].lens;
}

LLP_API llp_uint32 llp_message_next(llp_mes* lm, llp_uint32 idx, field_type* ft_out)
{
	llp_uint32 i;
	t_Mfield* tmd_p = NULL;
	check_null(lm, 0);
	check_null(ft_out, 0);
	
	for(i=(idx-1); i<lm->field_lens; i++)
	{
		if(lm->field_al[i].lens>0)
			break;
		idx++;
	}
	if(i>=lm->field_lens)
		return 0;
	
	tmd_p = &lm->d_mes->message_tfl[idx-1];
	
	// set field name
	ft_out->name = tmd_p->field_name;
	// set type
	ft_out->type = tag_type(tmd_p->tag);
	// set size
	ft_out->size = lm->field_al[i].lens;
	
	return idx+1;
}