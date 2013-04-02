#include "../llp.h"

#define llp_mes struct llp_mes
#define llp_env struct llp_env

static void dump_mes(llp_mes* lm);


// test message api
static void test_mes(llp_env* env)
{
	int i=0;
	slice* p;
	byte data[] = {1, 2, 3, 4, 5, 6};
	slice sl = {data, data, sizeof(data)};
	llp_mes* lm = llp_message_new(env, "testM");
	llp_mes* lms = llp_message_new(env, "testM");

	printf("mes_name = %s\n", llp_message_name(lm));
	llp_Wmes_integer(lm, "aa", -123456);
	llp_Wmes_string(lm, "bb", "test_string");
	llp_Wmes_real(lm, "cc", (float)(-345.67124));
	llp_Wmes_bytes(lm, "dd", &sl);
	llp_Wmes_integer(lm, "mm", 77);
	llp_Wmes_integer(lm, "mm", 88);
	llp_Wmes_integer(lm, "mm", 99);

	{
		llp_mes* lms = llp_Wmes_message(lm, "tt");
		llp_Wmes_string(lms, "tm_1", "self");	
	}

	printf("here?\n");
	llp_in_message(llp_out_message(lm), lms);
	print("%d\n%s\n%lf\n",
			(int)llp_Rmes_integer(lms, "aa", 0),
			llp_Rmes_string(lms, "bb", 0),
			llp_Rmes_real(lms, "cc", 0)
		);

	p=llp_Rmes_bytes(lms, "dd", 0);
	for(i=0; i<p->sp_size; i++){
		print("%d ", p->sp[i]);
	}

	{
		int i=0;
		llp_mes* lm = NULL;
		for(i=0; i<llp_Rmes_size(lms, "mm"); i++){
			printf("mm[%d] = %lld\n", i, llp_Rmes_integer(lms, "mm", i));
		}

		lm = llp_Rmes_message(lms, "tt", 0);
		printf("tt.tm_1 = %s\n tt.tm_2=%lld\n", llp_Rmes_string(lm, "tm_1", 0), llp_Rmes_integer(lm, "tm_2", 0));
	}

	dump_mes(lms);
	llp_message_free(lm);
	llp_message_free(lms);
}

// test dump message api
static void dump_mes(llp_mes* lm)
{
	static char* llpts[] = {
		"integer",
		"real",
		"string",
		"bytes",
		"message"
	};

	int idx=1;
	field_type ft = {0};

	printf("\n-------mes_name = %s---------\n", llp_message_name(lm));

	for(idx=1; 0!=(idx=llp_message_next(lm, idx, &ft)); ){
		printf("field_name = %s field_type=%s , field_size=%d\n", ft.name, llpts[ft.type], ft.size);
	}
}


int main(void)
{
	llp_env* env = NULL;
	env = llp_new_env();

	llp_reg_mes(env, "test.mes.lpb");
	
	test_mes(env);
//	dump_mes(env);

	llp_free_env(env);
	print_mem();
	return 0;
}