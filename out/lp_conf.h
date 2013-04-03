#ifndef _LP_CONF_H_
#define _LP_CONF_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef  WIN32
#include <stdint.h>
#endif


#define LLP_API extern

enum{
	LP_FAIL,
	LP_TRUE,
	LP_NIL,
	LP_EXIST,
	LP_END
};


#ifdef WIN32
typedef signed __int32		llp_int32;
typedef unsigned __int32	llp_uint32;
typedef signed __int64		llp_int64;
typedef unsigned __int64	llp_uint64;

#else
typedef int32_t				llp_int32;
typedef uint32_t			llp_uint32;
typedef int64_t				llp_int64;
typedef uint64_t			llp_uint64;
#endif

typedef float				llp_float32;
typedef double				llp_float64;
typedef llp_int64			llp_integer;
typedef	llp_float64			llp_real;

extern size_t mem;
#undef  malloc
#undef free

//#define malloc(s)	 lp_malloc(s,  __FILE__,  __LINE__)
#define malloc      lp_malloc
#define free		lp_free

typedef FILE*   f_handle;
#define f_open  fopen
#define f_close fclose
#define f_read  fread
#define f_write fwrite  

typedef unsigned char byte;
typedef struct _slice{
	byte* sp;
	byte* b_sp;
	unsigned int sp_size;
}slice;


#define	 check_value(e, qe, r)	do{if((e)==(qe)) return ((r));}while(0)	
#define	 check_fail(e, r)		check_value(e, LP_FAIL, r)
#define  check_null(e, r)		check_value(e, NULL, r)	
#define  print					printf

//void* lp_malloc(size_t len, char* file, int line);
void* lp_malloc(size_t len);
void lp_free(void* p);
long f_size( FILE *fp);

void print_mem();

#endif
