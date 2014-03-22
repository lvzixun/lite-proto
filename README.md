## Introduction
a simple and fast message data serialization DSL & libary. similar to google protobuf. but without code generation.

## how do use it ?
you can use make command.
```
make  clean
make  #gcc will check the system, and select window or linux. 

# other system
make macos   # mac osx
make android # android
```
will generate a  the name of `out` folder. you will see the file list:
```
libllp.a         # lp static library
libllpjava.so    # jni share library

lp               # lite-proto interpreter
                  
                 # the head files
llp.h
lp_conf.h
```
## `lp.lua`
I implement new parse mes file by lua lpeg.
you can input command `lua lp.lua -h`.
for detil at 
```
./lp_inter/lp.lua           # generating .mes.lpb file
./lp_inter/parse.lua        # parse .mes file to ast
```

## Quick example
First of all, you have to define a mes file:
```
#  testMes.mes
message testM {
	integer aa;
	string  bb;
	real    cc;
	bytes   dd;
	
	message TM{
		string tm_1;
		integer tm_2;
	}

	TM tt;
	integer[] mm;
}
```
you use lp interpreter compiled the testMes.mes file.
```
lp -o testMes.mes

# generated  testMes.mes.lpb file.
```
the c codes:
~~~~.c

env = llp_new_env();
llp_reg_mes(env, "test.mes.lpb");

llp_mes* lm = llp_message_new(env, "testM");
llp_Wmes_integer(lm, "aa", -123456);
llp_Wmes_string(lm, "bb", "test_string");
llp_Wmes_real(lm, "cc", (float)(-345.67124));

{
	llp_mes* lms = llp_Wmes_message(lm, "tt");
	llp_Wmes_string(lms, "tm_1", "self");	
}

llp_message_free(lm);
llp_free_env(env);
~~~~
## message c API
the read and write message api is defined [`llp.h`](https://github.com/lvzixun/lite-proto/blob/master/llp.h) head file. 

BTW, see llp_test/lp_test.c for detail.

## binding
the lite-proto currently to probide lua and java binding.

`llpjava` for java jni binding.

`llp/llplua.c & llp/llplua.h` for lua binding.

## wiki 

wiki at : https://github.com/lvzixun/lite-proto/wiki

the google code update info: http://code.google.com/p/lite-proto/

## question 
send mail to lvzixun@gmail.com

