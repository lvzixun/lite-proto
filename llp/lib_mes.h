#ifndef  _LIB_MES_H_
#define  _LIB_MES_H_
#include "../lp_conf.h"
#include "lib_lp.h"
#include "lib_al.h"


typedef struct llp_mes{
	t_def_mes*	 d_mes;			// message body struct
	llp_array*   field_al;		// field data
	llp_uint32	 field_lens;

	slice		 sio;			// out 
	slice		 sret;			//  ret slice
}llp_mes;

int _llp_Wmes(llp_mes* lm, int inx, byte v_type, void* msd);
#endif
