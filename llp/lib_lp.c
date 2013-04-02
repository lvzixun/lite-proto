#include "../llp.h"
#include "lib_lp.h"
#include "../lp_conf.h"
#include "lib_mes.h"
#include "lib_io.h"

static int llp_reg_mes_value(llp_env* env, slice* sl);
static int llp_read_message(llp_env* env, char** out_name, slice* sl);


LLP_API llp_env* llp_new_env()
{
	llp_env* ret = NULL;
	ret=(llp_env*)malloc(sizeof(llp_env));
	
	// new message table
	ret->dmes = lib_map_new();
	// new string pool
	ret->mesN = lib_stringpool_new();
	return ret;
}

LLP_API void llp_free_env(llp_env* p)
{
	if(p)
	{
		lib_Mmap_free(p->dmes);
		lib_stringpool_free(p->mesN);
		free(p);
	}
}

LLP_API int llp_reg_Smes(llp_env* env, slice* buff)
{
	check_null(env, LP_FAIL);
	check_null(buff, LP_FAIL);
	check_null(buff->b_sp, LP_FAIL);
	if(buff->sp_size == 0) return LP_FAIL;
	
	check_fail( llp_reg_mes_value(env, buff), LP_FAIL);
	return LP_TRUE;
}

LLP_API int llp_reg_mes(llp_env* env, char* lpb_file_name)
{
	long file_size =0;
	slice sl = {0};
	f_handle fd = f_open(lpb_file_name, "rb");
	file_size = f_size(fd);
	if(file_size<=0)	return LP_FAIL;
	check_null(env, LP_FAIL);
	check_null(lpb_file_name, LP_FAIL);
	check_null(fd, LP_FAIL);
	
	sl.sp_size = (unsigned int )file_size;
	sl.sp = (byte*)malloc(sl.sp_size);
	sl.b_sp = sl.sp;
	f_read(sl.sp, 1, sl.sp_size, fd);
	f_close(fd);
	check_fail( llp_reg_mes_value(env, &sl), 
				(free(sl.b_sp), LP_FAIL)		
			  );

	free(sl.b_sp);
	return LP_TRUE;
}

static int llp_reg_mes_value(llp_env* env, slice* sl)
{
	char* out_name = NULL;
	check_null(sl, LP_FAIL);
	check_null(sl->sp, LP_FAIL);

	for(;;)
	{
		switch(llp_read_message(env, &out_name, sl))
		{
		case LP_FAIL:
			// des
			return LP_FAIL;
		case LP_END:
			return LP_TRUE;
		}
	}
}


static int llp_read_field(llp_env* env, t_def_mes* des_mes, slice* sl)
{
	llp_uint32 i=0;
	des_mes->message_tfl = (t_Mfield*)malloc(sizeof(t_Mfield)*des_mes->message_count);
	memset(des_mes->message_tfl, 0, sizeof(t_Mfield)*des_mes->message_count);
	check_null( des_mes->message_field = lib_Fmap_new(des_mes->message_count) ,
				LP_FAIL	
			  );

	for(i=0; i<des_mes->message_count; i++)
	{
		char* f_name = NULL;
		check_fail(sl_Rbyte(sl, &des_mes->message_tfl[i].tag), LP_FAIL);
		if(tag_type(des_mes->message_tfl[i].tag) == LLPT_MESSAGE)
		{
			byte ei = 0;
			char* fms = NULL;
			t_def_mes* mes_p = NULL;
			check_fail(sl_Rbyte(sl, &ei), LP_FAIL);
			check_fail(sl_Rstr(sl, &fms), LP_FAIL);
			mes_p = lib_Mmap_find(env->dmes, fms);
			if(mes_p == NULL && !ei)
			{
				mes_p = lib_Mmap_add(env->dmes, lib_stringpool_add(env->mesN, fms));
				check_null(mes_p, LP_FAIL);
			}else if(mes_p == NULL && ei)
			{
				// not find extern message
				return LP_FAIL;
			}
			des_mes->message_tfl[i].tms = mes_p;
		}
		
		check_fail(sl_Rstr(sl, &f_name), LP_FAIL);
		check_fail( lib_Fmap_add(des_mes->message_field, 
			  		des_mes->message_tfl[i].field_name = lib_stringpool_add(env->mesN, f_name), i),
					LP_FAIL
				  );
	}

	return LP_TRUE;
}

static int llp_read_message(llp_env* env, char** out_name, slice* sl)
{
	t_def_mes* mes_p = NULL;
	check_sl(sl);
	check_fail(sl_Rstr(sl, out_name), LP_FAIL);										// read message name
	*out_name = lib_stringpool_add(env->mesN, *out_name);							// add name
	check_null(mes_p=lib_Mmap_add(env->dmes, *out_name), LP_FAIL);					// begin add message body
	mes_p->message_name = *out_name;												// set message name
	check_fail(sl_Ruint(sl, &(mes_p->message_id)), LP_FAIL);						// read id
	check_fail(sl_Ruint(sl, &(mes_p->message_count)), LP_FAIL);						// read message count
	check_fail(llp_read_field(env, mes_p, sl), LP_FAIL);							// read field

	return LP_TRUE;
}


