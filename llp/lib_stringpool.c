#include "lp_conf.h"
#include "lib_stringpool.h"
#include "lib_table.h"

//  read from pbc 


// the string's lens at string pool
#define STR_PAGE_SIZE		 128

string_pool* lib_stringpool_new()
{
	string_pool* ret = (string_pool*)malloc(sizeof(string_pool));
	ret->pool = (string_pool_slot*)malloc(sizeof(string_pool_slot) + STR_PAGE_SIZE);
	ret->string_table = lib_map_new();

	ret->pool->str = (char*)(ret->pool+1);
	ret->pool->len = 0;
	ret->pool->next = NULL;
	
	return ret;
}

void lib_stringpool_free(string_pool* sp)
{
	string_pool_slot* ssp = NULL;
	if(sp==NULL)
		return;

	lib_map_free(sp->string_table);
	ssp = sp->pool;
	while(ssp)
	{
		string_pool_slot* next = ssp->next;
		free(ssp);
		ssp = next;
	}

	free(sp);
}


static char* _lib_stringpool_add(string_pool_slot* sp, const char* str, size_t s_len)
{
	string_pool_slot* ret = NULL;
	size_t len=s_len +1;
	check_null(sp, NULL);
	check_null(str, NULL);
	check_fail(str[0], NULL);
	
	if(len <= STR_PAGE_SIZE - sp->len)
	{
		char* ret = sp->str + sp->len;
		memcpy(ret, str, len);
		sp->len +=len;
		return ret;
	}

	if(len > STR_PAGE_SIZE)
	{
		ret = (string_pool_slot*)malloc(sizeof(string_pool_slot) + len);
		ret->str = (char*)(ret+1);
		ret->len = len;
		ret->next = sp->next;
		sp->next = ret;
		memcpy(ret->str, str, len);

		return ret->str;
	}
	

	ret = (string_pool_slot*)malloc(sizeof(string_pool_slot) + STR_PAGE_SIZE);
	ret->str = sp->str;
	ret->len = sp->len;
	ret->next = sp->next;

	sp->next = ret;
	sp->str = (char*)(ret+1);
	sp->len = len;
	memcpy(sp->str, str, len);
	return sp->str;
}


char* lib_stringpool_add(string_pool* sp, const char* str)
{
	size_t len =0;
	char** ret = NULL;
	check_null(sp, NULL);
	check_null(str, NULL);

	len = strlen(str)+1;
	ret = (char**)lib_map_find(sp->string_table, str);

	if(ret == NULL)
	{
		llp_kv kv = {0};
		char* add_str = _lib_stringpool_add(sp->pool, str, len);
		kv.key = add_str;
		kv.vp = add_str;
		lib_map_add(sp->string_table, &kv);
		return add_str;
	}

	return *ret;
}

/*
void dump_stringpool(string_pool* sp)
{
	int i=0;
	char* str = NULL;
	string_pool_slot* ssp = sp->pool;
	while(ssp)
	{
		print("pool[%d] {size=%d str_size=%d off_size = %d}\n", i, STR_PAGE_SIZE, ssp->len, STR_PAGE_SIZE - ssp->len);
		str = ssp->str;
		while(((size_t)(str - ssp->str))<ssp->len)
		{
			print("    %s\n", str);
			str =str + 1 + strlen(str);
		}
		print("pool[%d]----------\n\n", i);
		ssp = ssp->next;
		i++;
	}
}
*/

