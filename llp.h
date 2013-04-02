#ifndef _LLP_H_
#define _LLP_H_
#include "lp_conf.h"


struct llp_env; 
struct llp_mes; 

// -------- llp type
#define LLPT_INTEGER  0
#define LLPT_REAL     1
#define LLPT_STRING   2
#define LLPT_BYTES    3
#define LLPT_MESSAGE  4

// field type
typedef struct _field_type{
	int   type;			// field type
	int   size;			// field size
	char* name;			// field name	
} field_type;

// --------env
struct llp_env* llp_new_env();
void llp_free_env(struct llp_env* p);

// --------registration message from .lpb files (not thread-safe)
int llp_reg_mes(struct llp_env* env, char* lpb_file_name);
// --------registration message from slice (not thread-safe)
int llp_reg_Smes(struct llp_env* env, slice* buff);

// --------new/delete a message object
struct llp_mes*  llp_message_new(struct llp_env* env, char* mes_name);
void llp_message_clr(struct llp_mes* lm);
void  llp_message_free(struct llp_mes* lm);
// --------get a message object's name
char* llp_message_name(struct llp_mes* lm);
/* --------dump field type from message object
** lm	   :message object
** idx     :field index (you can set 1 when begin dump)
** ft_out  :Parameter return 
** return  : next field index (dump end when return 0)
*/ 
llp_uint32 llp_message_next(struct llp_mes* lm, llp_uint32 idx, field_type* ft_out);


// --------write a message object
int llp_Wmes_integer(struct llp_mes* lm, char* field_name, llp_integer number);
int llp_Wmes_real(struct llp_mes* lm, char* field_name, llp_real number);
int llp_Wmes_string(struct llp_mes* lm, char* field_name, char* str);
int llp_Wmes_bytes(struct llp_mes* lm, char* field_name, slice* sl);
struct llp_mes* llp_Wmes_message(struct llp_mes* lm, char* field_name);

// ---------read a message object
llp_integer llp_Rmes_integer(struct llp_mes* lm, char* field_name, unsigned int al_inx);
llp_real llp_Rmes_real(struct llp_mes* lm, char* field_name, unsigned int al_inx);
slice* llp_Rmes_bytes(struct llp_mes* lm, char* field_name, unsigned int al_inx);
char* llp_Rmes_string(struct llp_mes* lm, char* field_name, unsigned int al_inx);
struct llp_mes* llp_Rmes_message(struct llp_mes* lm, char* field_name, unsigned int al_inx);
llp_uint32 llp_Rmes_size(struct llp_mes* lm, char* field_name);

// ------- out/in a message body
slice* llp_out_message(struct llp_mes* lms);
int llp_in_message(slice* in, struct llp_mes* lms);

#endif
