#include "lib_table.h"
#include "lib_lp.h"
#include "lp_conf.h"

static size_t calc_hash(const char* name);
#define DEF_MAP_LENS		256

// read from pbc
static size_t calc_hash(const char* name)
{
	size_t len = strlen(name);
	size_t h = len;
	size_t step = (len>>5)+1;
	size_t i;
	for (i=len; i>=step; i-=step)
		h = h ^ ((h<<5)+(h>>2)+(size_t)name[i-1]);
	return h;
}

llp_map* lib_map_new()
{
	llp_map* ret = (llp_map*)malloc(sizeof(llp_map));
	ret->size = DEF_MAP_LENS;
	ret->cap = 0;
	ret->table = (llp_slot*)malloc(sizeof(llp_slot)*DEF_MAP_LENS);
	memset(ret->table, 0, sizeof(llp_slot)*DEF_MAP_LENS);

	return ret;
}


void lib_map_free(llp_map* l_map)
{
	if(l_map)
	{
		free(l_map->table);
		free(l_map);
	}
}


static void lib_map_reflush(llp_map* l_map)
{
	size_t i, new_size=l_map->size+DEF_MAP_LENS, hash;
	llp_slot* new_slot = (llp_slot*)malloc(sizeof(llp_slot)*new_size);
	memset(new_slot, 0, sizeof(llp_slot)*new_size);

	for(i=0; i<l_map->size; i++)
	{
		hash = l_map->table[i].hash % new_size;
		if(new_slot[hash].key == NULL)
		{
			new_slot[hash].key = l_map->table[i].key;
			new_slot[hash].hash = l_map->table[i].hash;
			new_slot[hash].vp = l_map->table[i].vp;
		}
		else
		{
			size_t emp = hash;
			for(;new_slot[emp].key!=NULL;)
				emp = (emp + 1) % new_size;

			new_slot[emp].key = l_map->table[i].key;
			new_slot[emp].hash = l_map->table[i].hash;
			new_slot[emp].vp = l_map->table[i].vp;
			new_slot[emp].next = new_slot[hash].next;
			new_slot[hash].next = emp+1;
		}
	}

	free(l_map->table);
	l_map->size = new_size;
	l_map->table = new_slot;
}


void** lib_map_find(llp_map* l_map, const char* key)
{
	size_t hash_full, hash;
	check_null(l_map, NULL);
	check_null(key, NULL);
	if(!(l_map->size))	return NULL;
	hash_full = calc_hash(key);
	hash = hash_full % l_map->size;

	for(;l_map->table[hash].key!=NULL;)
	{	
		if(l_map->table[hash].hash==hash_full && strcmp(key, l_map->table[hash].key)==0)
			return &(l_map->table[hash].vp);
		if(l_map->table[hash].next==0)
			return NULL;
		hash = l_map->table[hash].next -1;
	}

	return NULL;
}


int lib_map_add(llp_map* l_map, llp_kv* kv)
{
	size_t hash, sh;
	check_null(l_map, LP_FAIL);
	check_null(kv, LP_FAIL);
	check_null(kv->key, LP_FAIL);
	
	hash = calc_hash(kv->key);

	if(l_map->cap >= l_map->size)
		lib_map_reflush(l_map);

	sh = hash % l_map->size;
	if(l_map->table[sh].key == NULL)
	{
		l_map->table[sh].key = kv->key;
		l_map->table[sh].hash = hash;
		l_map->table[sh].vp = kv->vp;
	}
	else
	{
		size_t emp = sh;
		size_t idx = sh;
		for( ;; )
		{
			if(l_map->table[idx].hash == hash && strcmp(l_map->table[idx].key, kv->key)==0)
				return LP_EXIST;
			if(l_map->table[idx].next==0)
				break;
			idx = l_map->table[idx].next -1;
		}

		for( ;l_map->table[emp].key!=NULL; )
			emp=(emp +1)%l_map->size;

		l_map->table[emp].hash = hash;
		l_map->table[emp].key = kv->key;
		l_map->table[emp].vp = kv->vp;
		l_map->table[emp].next = l_map->table[sh].next;
		l_map->table[sh].next = emp+1;
	}

	l_map->cap++;
	return LP_TRUE;
}


t_def_mes* lib_Mmap_add(llp_map* l_map, char* message_name)
{
	int ret=0;
	llp_kv kv = {0};
	kv.key = message_name;
	kv.vp = malloc(sizeof(t_def_mes));
	memset(kv.vp, 0, sizeof(t_def_mes));

	ret=lib_map_add(l_map, &kv);
	if(ret == LP_FAIL)
	{
		free(kv.vp);
		return NULL;
	}
	else if(ret==LP_EXIST)
	{
		t_def_mes* _vp = NULL;
		free(kv.vp);
		_vp = kv.vp=*(lib_map_find(l_map, message_name));
		// the message is already regedited
		if(_vp->message_field != NULL && _vp->message_tfl != NULL)
			return NULL;
	}

	return (t_def_mes*)kv.vp;
}

t_def_mes* lib_Mmap_find(llp_map* l_map, char* message_name)
{
	void** ret = NULL;
	check_null(l_map, NULL);
	check_null(message_name, NULL);

	check_null(ret = lib_map_find(l_map, message_name), NULL);
	return (t_def_mes*)(*ret);
}

void lib_Mmap_free(llp_map* l_map)
{
	size_t i;
	t_def_mes* temp = NULL;
	if(l_map==NULL)
		return;

	for(i=0; i<l_map->size; i++)
	{
		if(l_map->table[i].key == NULL)
			continue;

		temp = (t_def_mes*)l_map->table[i].vp;
		lib_Fmap_free(temp->message_field);
		free(temp->message_tfl);
		free(temp);
	}

	lib_map_free(l_map);
}

field_map* lib_Fmap_new(size_t size)
{
	field_map* ret = (field_map*)malloc(sizeof(field_map) + size*sizeof(field_slot));
	check_null(ret, NULL);
	
	ret->size = size;
	ret->slot = (field_slot*)(ret + 1);
	memset(ret->slot, 0, sizeof(field_slot)*size);

	return ret;
}

void lib_Fmap_free(field_map* f_map)
{
	if(f_map)
		free(f_map);
}

int lib_Fmap_add(field_map* f_map,  char* field_name, int id)
{
	size_t hash, emp, sh, hash_full;
	check_null(f_map, LP_FAIL);
	check_null(field_name, LP_FAIL);
	hash_full =calc_hash(field_name);
	hash = hash_full % f_map->size;
	
	sh = hash;
	if(f_map->slot[hash].field_name == NULL)
	{
		f_map->slot[hash].field_name = field_name;
		f_map->slot[hash].id = id;
		f_map->slot[hash].hash = hash_full;
	}
	else
	{
		for( ;; )
		{	
			if(f_map->slot[sh].hash== hash_full && strcmp(f_map->slot[sh].field_name, field_name)==0)
				return LP_EXIST;
			if(f_map->slot[sh].next==0)
				break;
			
			sh = f_map->slot[sh].next -1;
		}
		
		emp = hash;
		for(;f_map->slot[emp].field_name!=NULL;)
			emp = (emp + 1)%f_map->size;
		
		f_map->slot[emp].next = f_map->slot[hash].next;
		f_map->slot[hash].next = emp + 1;
		f_map->slot[emp].field_name = field_name;
		f_map->slot[emp].id = id;
		f_map->slot[emp].hash = hash_full;
	}

	return LP_TRUE;
}

int* lib_Fmap_find(field_map* f_map, char* field_name)
{
	size_t hash, hash_full;
	check_null(f_map, NULL);
	check_null(field_name, NULL);
	if(!(f_map->size))	return NULL;
	hash_full = calc_hash(field_name);
	hash = hash_full % f_map->size;

	for(;f_map->slot[hash].field_name!= NULL;)
	{
		if(f_map->slot[hash].hash==hash_full && strcmp(field_name, f_map->slot[hash].field_name)==0)
			return &f_map->slot[hash].id;
		if(f_map->slot[hash].next==0)
			return NULL;
		hash = f_map->slot[hash].next -1;
	}
	return NULL;
}

char* malloc_string(char* str)
{
	int len = 0;
	char* ret = NULL;
	check_null(str, NULL);
	len = strlen(str);
	ret = (char*)malloc(len+1);
	memcpy(ret, str, len);
	ret[len] = 0;

	return ret;
 }

slice* malloc_slice(slice* sl)
{
	slice* ret = (slice*)malloc(sizeof(*ret));
	
	check_null(ret, NULL);
	check_null(sl, NULL);
	ret->b_sp=ret->sp=(byte*)malloc(sl->sp_size);
	if(ret->b_sp==NULL)
	{
		free(ret);
		return NULL;
	}
	ret->sp_size = sl->sp_size;
	memcpy(ret->sp, sl->sp, ret->sp_size);
	return ret;
}

