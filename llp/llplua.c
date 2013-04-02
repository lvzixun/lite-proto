#include "llplua.h"

#define llp_env struct llp_env
#define llp_mes struct llp_mes

typedef struct _l_env{
	llp_env* env;
	byte	is_self;
}l_env;

typedef struct _l_mes {
	llp_mes* mes;
}l_mes;

#define LLP_TS			"llp"
#define ENV_US			"env"
#define LM_CO			"_cobj"
#define LM_SL			"sl"

#define check_llpenv(L)	do{	\
							luaL_checktype(L, 1, LUA_TTABLE);	\
							lua_getfield(L, 1, ENV_US);	\
							luaL_checktype(L, -1, LUA_TUSERDATA);	\
						}while(0)

#define get_llpenv(L)	(llp_env*)(((l_env*)lua_touserdata(L, -1))->env)


#define check_llpmes(L) do{	\
							luaL_checktype(L, 1, LUA_TTABLE);	\
							lua_getfield(L, 1, LM_CO);	\
							luaL_checktype(L, -1, LUA_TUSERDATA);	\
						}while(0)

#define get_llpmes(L)	(llp_mes*)( ((l_mes*)lua_touserdata(L, -1))->mes)

static void _llpL_new_mes(lua_State* L, char* mes_name, llp_mes* lm, lua_CFunction gc_func);
static void _llpL_dump_data(lua_State* L, llp_mes* lm);
static void _llpL_encode_table(lua_State* L, llp_mes* lm,  char* deep_name);


// message obj gc function
static int _llpL_free_mes(lua_State* L)
{
	llp_mes* lm = ((l_mes*)lua_touserdata(L, -1))->mes;
	llp_message_free(lm);
	return 0;
}

// dump type not repeated
static void _llpDump_ltype(lua_State* L, field_type* ftp, llp_mes* lm, unsigned int idx)
{
	int tidx= lua_gettop(L);

	switch(ftp->type)
	{
	case LLPT_REAL:
		lua_pushnumber(L, (lua_Number)llp_Rmes_real(lm, ftp->name, idx));
		break;
		break;
	case  LLPT_INTEGER:
		lua_pushinteger(L, (lua_Integer)llp_Rmes_integer(lm, ftp->name, idx));
		break;
	case  LLPT_STRING:
		lua_pushstring(L, (const char*)llp_Rmes_string(lm, ftp->name, idx));
		break;
	case  LLPT_BYTES:
		lua_pushlightuserdata(L, llp_Rmes_bytes(lm, ftp->name, idx));
		break;
	case  LLPT_MESSAGE:
		{
			lua_newtable(L);
			_llpL_dump_data(L, llp_Rmes_message(lm, ftp->name, idx));
		}
		break;
	default: 
		luaL_error(L, "[dump_value error]: the type[%d] is error!\n", (int)(ftp->type));
		return;
	}
}


// dump data
static void _llpL_dump_data(lua_State* L, llp_mes* lm)
{
	int tidx = lua_gettop(L);
	llp_uint32 idx=1;
	field_type ft;

	while( (idx=llp_message_next(lm, idx, &ft))!=0 )
	{	
		if(ft.size==1)
		{
			_llpDump_ltype(L, &ft, lm, 0);
			lua_setfield(L, tidx, ft.name);
		}
		else if(ft.size>1)
		{
			int i;
			lua_newtable(L);
			for(i=0; i<ft.size; i++)
			{
				lua_pushnumber(L, (lua_Number)(i+1));
				_llpDump_ltype(L, &ft, lm, (unsigned int)i);
				lua_settable(L, -3);
			}
// 			lua_pushnumber(L, (lua_Number)size);
// 			lua_setfield(L, -2, "size");
			lua_setfield(L, tidx, ft.name);
		}
	}
}

static void _llpL_new_mes(lua_State* L, char* mes_name, llp_mes* lm, lua_CFunction gc_func)
{
	slice* sl=NULL;
	int tidx;
	l_mes* l_mp;

	// set message table
	lua_newtable(L);
	tidx = lua_gettop(L);
	
	// set message name
	lua_pushstring(L, mes_name);
	lua_setfield(L, -2, "message_name");

	// set message obj
	l_mp = lua_newuserdata(L, sizeof(l_mes));
	l_mp->mes = lm;

	if(gc_func)
	{
		// set message obj gc
		lua_newtable(L);
		lua_pushstring(L, "__gc");
		lua_pushcfunction(L, gc_func);
		lua_rawset(L, -3);
		lua_setmetatable(L, -2);
	}
	lua_setfield(L, tidx, LM_CO);
}

//  free env
static int _llpL_close(lua_State* L)
{
	l_env* le = lua_touserdata(L, -1);
	
	if(le->is_self)
		llp_free_env(le->env);
	return 0;
}

#define ENCODE_ERROR(s)	( luaL_error(L, "[encode error]: write key '%s' is error.", s) )


static int _llpL_encode_value(lua_State* L, llp_mes* lm, int rt, char* field_name)
{
	int ret_type = lua_type(L, -1);
	
	if(rt!=LUA_TNIL && ret_type!=rt)
		ENCODE_ERROR("type is error");

	switch(ret_type)		// value
	{
	case LUA_TNUMBER:
		{
			lua_Number  nn = lua_tonumber(L, -1);
			lua_Integer ni = lua_tointeger(L, -1);
			
			
			if(nn == (lua_Number)ni)	// the number is integer
			{
				if(llp_Wmes_integer(lm, field_name, ni)==LP_FAIL)
					ENCODE_ERROR(field_name);
			}
			else						// the number is real
			{
				if(llp_Wmes_real(lm, field_name, nn)==LP_FAIL)
					ENCODE_ERROR(field_name);
			}
			break;
		}
	case LUA_TSTRING:
		if(llp_Wmes_string(lm, field_name, (char*)lua_tostring(L, -1)) == LP_FAIL)
			ENCODE_ERROR(field_name);
		break;
	case LUA_TTABLE:
		{
			int top = lua_gettop(L);
			lua_pushnumber(L, 1);
			lua_gettable(L, -2);
			
			if(lua_type(L, -1) == LUA_TTABLE)
			{
				lm = llp_Wmes_message(lm, field_name);
				if(lm==NULL)
					ENCODE_ERROR(field_name);
			}

			lua_settop(L, top);
			_llpL_encode_table(L, lm, field_name);
		}
		break;
	default:
		luaL_error(L, "[encode error]: write key '%s' is not allow.", field_name);
	}

	return ret_type;
}

static void _llpL_encode_table(lua_State* L, llp_mes* lm,  char* deep_name)
{
	int rt = LUA_TNIL;
	lua_pushnil(L);
	while(lua_next(L, -2))
	{
		switch(lua_type(L, -2))		// key
		{
		case LUA_TNUMBER:
			{
				if(deep_name == NULL)
					ENCODE_ERROR("is number");
				else
					rt = _llpL_encode_value(L, lm, rt, deep_name);
			}
			break;
		case LUA_TSTRING:
			{
				char* field_name = (char*)lua_tostring(L, -2);
				_llpL_encode_value(L, lm, LUA_TNIL, field_name);
			}
			break;
		default:
			luaL_error(L, "[encode error]: the key not allow! key's type must string/number.");
			return;
		}
		lua_pop(L, 1);
	}
}


// clear 
static int llpL_clear(lua_State* L)
{
	l_mes* l_m = NULL;
	luaL_checktype(L, 2, LUA_TTABLE);
	lua_getfield(L, 2, LM_CO);
	luaL_checktype(L, -1, LUA_TUSERDATA);

	l_m = (l_mes*)(lua_touserdata(L, -1));
	llp_message_free(l_m->mes);
	l_m->mes = NULL;

	return 0;
}

// encode 
static int llpL_encode(lua_State* L)
{
	int top;
	char* mes_name;
	llp_env* env;
	llp_mes* lm;
	check_llpenv(L);
	env = get_llpenv(L);
	
	mes_name = (char*)luaL_checkstring(L, 2);
	lm = llp_message_new(env, mes_name);
	if(lm==NULL)
		luaL_error(L, "[encode error]: not find message '%s'.", mes_name);
	_llpL_new_mes(L, mes_name, lm, _llpL_free_mes);
	top = lua_gettop(L);

	// encode table
	lua_pushvalue(L, 3);
	_llpL_encode_table(L, lm, NULL);

	// set message out
	lua_pushvalue(L, top);
	return 	1;
}

// decode
static int llpL_decode(lua_State* L)
{
	l_mes* l_m = NULL;
	char* mes_name;
	llp_env* env;
	check_llpenv(L);
	env = get_llpenv(L);

	mes_name = (char*)luaL_checkstring(L, 2);
	luaL_checktype(L, 3, LUA_TTABLE);
	lua_getfield(L, 3, LM_CO);
	luaL_checktype(L, -1, LUA_TUSERDATA);

	l_m = (l_mes*)(lua_touserdata(L, -1));
	if(l_m->mes==NULL)
		luaL_error(L, "[decode error]: decode message '%s' is error.", mes_name);
	
	lua_newtable(L);
	_llpL_dump_data(L, l_m->mes);

	return 1;
}

// push a message obj with no gc function
void lua_pushlm(lua_State* L, llp_mes* lm)
{
	if(lm==NULL)
		return;

	_llpL_new_mes(L, llp_message_name(lm), lm, NULL);
}

// pop a message obj
llp_mes* lua_tolm(lua_State* L, int idx)
{
	l_mes* l_m=NULL;
	if(lua_type(L, idx)!=LUA_TTABLE)
		return NULL;
	lua_getfield(L, idx, LM_CO);
	if(lua_type(L, -1)!=LUA_TLIGHTUSERDATA)
		return NULL;

	l_m=(l_mes*)(lua_touserdata(L, -1));
	return l_m->mes;
}

// reg message
static int llpL_reg_mes(lua_State* L)
{
	char* file_name;
	llp_env* env;
	
	check_llpenv(L);
	env = get_llpenv(L);

	file_name = (char*)lua_tostring(L, 2);
	if(llp_reg_mes(env, file_name) == LP_FAIL)
		luaL_error(L, "[reg_mes error]: '%s' file is regidet fail!\n", file_name);
	
	return 0;
}



int llpL_open(lua_State* L, llp_env* env)
{
	byte flag = LP_FAIL;
	int top, i;
	int llpt_idx;
	l_env* le ;
	luaL_Reg reg[] = {
		{"reg_message", llpL_reg_mes},
		{"decode", llpL_decode},
		{"encode", llpL_encode},
		{"clear", llpL_clear}
	};

	check_null(L, LP_FAIL);
	if(env==NULL)
	{
		env = llp_new_env();
		check_null(env, LP_FAIL);
		flag = LP_TRUE;
	}

	top = lua_gettop(L);
	
	// new llp table
	lua_newtable(L);
	llpt_idx = lua_gettop(L);

	// set env
	le=lua_newuserdata(L, sizeof(l_env));
	le->env = env;
	le->is_self = flag;

	// set metatable
	lua_newtable(L);
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, _llpL_close);
	lua_rawset(L, -3);
	lua_setmetatable(L, -2);
	lua_setfield(L, llpt_idx, "env");


	// Registration function
	for(i=0; i<sizeof(reg)/sizeof(luaL_Reg); i++)
	{
		lua_pushcfunction(L, reg[i].func);
		lua_setfield(L, llpt_idx, reg[i].name);
	}

	lua_setglobal(L, LLP_TS);
	lua_settop(L, top);
	return LP_TRUE;
}