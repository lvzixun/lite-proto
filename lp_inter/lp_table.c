#include "lp_table.h"
#include "../lp_conf.h"
#include "lp_list.h"
#include <memory.h>

#define  lp_hash(str)	_BKDRHash(str, TABLE_SIZE)
int lp_table_one_free(lp_table_one* lt_o);


unsigned int _BKDRHash(char *str, int len)
{
	unsigned int seed = 131; 
	unsigned int hash = 0;
	
	while (*str)
		hash = hash * seed + (*str++);
	
	return (hash & 0x7FFFFFFF)%(len);
}

int lp_table_new(lp_table* lp_t)
{
	check_null(lp_t, LP_FAIL);
	lp_t->table_len = 0;
	memset(lp_t->table_list, 0, sizeof(lp_table_one*));
	
	return LP_TRUE;
}

int lp_table_add(lp_table* lp_t, char* name, int value)
{
	lp_table_one* head = NULL;
	int inx = 0;
	check_null(lp_t, LP_FAIL);
	check_null(name, LP_FAIL);
	
	inx = lp_hash(name);
	head = lp_t->table_list[inx];
	if(head==NULL)
	{
		lp_t->table_list[inx] = (lp_table_one*)malloc(sizeof(lp_table_one));
		memset(lp_t->table_list[inx], 0, sizeof(lp_table_one));
		lp_t->table_list[inx]->name = lp_string_new(name);
		lp_t->table_list[inx]->value = value;
	}
	else
	{
		lp_table_one* back = NULL;
		while(head)
		{
			if(strcmp((char*)head->name.str.list_p, name)==0)
				return LP_EXIST;
			back = head;
			head = head->next;
		}
		back->next = (lp_table_one*)malloc(sizeof(lp_table_one));
		memset(back->next, 0, sizeof(lp_table_one));
		back->next->name = lp_string_new(name);
		back->next->value = value;
	}
	return LP_TRUE;
}

int* lp_table_query(lp_table* lp_t, char* name)
{
	lp_table_one* lt_o = NULL;
	int inx =0;
	check_null(lp_t, NULL);
	check_null(name, NULL);
	
	inx = lp_hash(name);
	lt_o = lp_t->table_list[inx];
	while(lt_o)
	{
		if(strcmp((char*)lt_o->name.str.list_p, name)==0)
			return &(lt_o->value);
		lt_o = lt_o->next;
	}
	return NULL;
}

int lp_table_free(lp_table* lp_t)
{
	int i=0;
	check_null(lp_t, LP_FAIL);
	for(i=0; i<TABLE_SIZE; i++)
	{
		if(lp_t->table_list[i])
		{
			lp_table_one* lt_o = lp_t->table_list[i];
			while(lt_o)
			{
				lp_table_one* nt = lt_o->next;
				lp_table_one_free(lt_o);
				lt_o = nt;
			}
			lp_t->table_list[i] = NULL;
		}
	}
	return LP_TRUE;
}

int lp_table_one_free(lp_table_one* lt_o)
{
	check_null(lt_o, LP_FAIL);
	lp_string_free(&lt_o->name);
	free(lt_o);
	return LP_TRUE;
}

lp_string lp_string_new(char* str)
{
	lp_string lp_s;
	memset(&lp_s, 0, sizeof(lp_s));

	check_null(str, lp_s);
	lp_list_new(&lp_s.str, 1, NULL, NULL);
	while(*str)
	{
		lp_list_add(&lp_s.str, (byte*)str);
		str++;
	}

	lp_list_add(&lp_s.str, (byte*)str);
	lp_s.str.list_len--;
	return lp_s;
}

lp_string* lp_string_cats(lp_string* lp_s, char* str)
{
	size_t i=0;
	check_null(lp_s, NULL);
	check_null(str, NULL);
	for(i=0; i<strlen(str)+1; i++)
		lp_list_add(&lp_s->str, (byte*)(str+i));
	lp_s->str.list_len--;
	return lp_s;
}

lp_string* lp_string_cat(lp_string* lp_s, char at_char)
{
	char a = 0;
	check_null(lp_s, NULL);
	
	lp_list_add(&lp_s->str, (byte*)(&at_char));
	lp_list_add(&lp_s->str, (byte*)(&a));
	lp_s->str.list_len--;
	return lp_s;
}

void lp_string_clear(lp_string* lp_s)
{
	if(lp_s==NULL)
		return;
	lp_s->str.list_len = 0;
}

void lp_string_free(lp_string* lp_s)
{
	if(lp_s==NULL)
		return;

	lp_list_free(&lp_s->str);
}



