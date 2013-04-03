#ifndef _LIB_LP_H_
#define _LIB_LP_H_
#include "../lp_conf.h"
#include "lib_table.h"
#include "lib_stringpool.h"


typedef enum _e_lpf{
	lpf_req,		// required
	lpf_rep			// repeated
}e_lpf;

typedef enum _e_lt{
	def_mes,		// message
	def_field		// field at message body 
}e_lt;

struct _t_def_mes;
typedef struct _t_Mfield{
	byte tag;					// tag
	char* field_name;			// field name
	struct _t_def_mes* tms;		// if type message 
}t_Mfield;

// def message body
typedef struct _t_def_mes{
	llp_uint32		message_id;			// message id
	char*			message_name;		// message name
	field_map*		message_field;		// field table
	t_Mfield*		message_tfl;		// field tag list
	llp_uint32		message_count;		// field count
}t_def_mes;


typedef struct llp_env{		
	llp_map* dmes;			// message body

	string_pool* mesN;		// string pool
}llp_env;


// #define DEF_MES_LEN		64
// #define DEF_DMES_LEN    128
 
#define  tag_type(t)	( ((byte)t)>>3 )
#define  tag_state(t)	( ((byte)(t)) & 0x07 ) 

#endif

