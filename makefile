ifeq ($(OS),Windows_NT)
 LP = lp.exe
 LLP=llp.lib
 LLPJAVA = llpjava.dll
 RM = del
 RMDIR = rmdir /S /Q
 CP = copy
 N = \\
 OUT = $(_LP) $(_LLP) $(_LLPJAVA)
 JNI = -I"$(JAVA_HOME)\include" -I"$(JAVA_HOME)\include\win32"
 DLLARG = -Wl,--kill-at -fPIC
else
 LP = lp
 LLP=libllp.a
 LLPJAVA = libllpjava.so
 RM = rm -rf
 RMDIR = $(RM)
 CP = cp
 N = //
 JNI = $(JDK)$(N)bin$(N)java
 OUT = $(_LP) $(_LLP) $(_LLPJAVA)
 JNI = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
 DLLARG = -Wl -fPIC
 CFLAGS += -fPIC
endif

CC = gcc
CFLAGS += -O2 -Wall  
AR = ar rcu

# lite-proto interpreter exec 
LP_PATH =  .$(N)lp_inter
LP_INTERPRETER_O = lp.o lp_file.o lp_lex.o lp_list.o lp_parse.o lp_table.o 
_LP = .$(N)$(LLP_OUT)$(N)$(LP)

# lite-proto lib
LLP_OUT = out
LLP_PATH = .$(N)llp
LLP_O = lib_al.o lib_io.o lib_lp.o lib_mes.o lib_table.o lib_stringpool.o
_LLP = .$(N)$(LLP_OUT)$(N)$(LLP)

# lite-proto llpJava dll
LLP_JO =  llpjava.o
_LLPJAVA = .$(N)$(LLP_OUT)$(N)$(LLPJAVA)

LP_CONF_O = lp_conf.o
BUILD_LLP_O = $(foreach s, $(LLP_O), $(LLP_PATH)$(N)$(s))
BUILD_LLP_JO = .$(N)$(LLP_PATH)$(N)$(LLP_JO)
BUILD_LP_O  = $(foreach s, $(LP_INTERPRETER_O), $(LP_PATH)$(N)$(s)) 
BULID_LLP_JC = $(foreach s, $(BUILD_LLP_JO), $(basename $(s)).c)


AND_JNI = android$(N)jni
AND_JNIP = $(AND_JNI)$(N)src

ALL : BUILD_PATH  $(OUT)
	$(CP) lp_conf.h  .$(N)$(LLP_OUT)
	$(CP) llp.h   .$(N)$(LLP_OUT)
	cp -rf ./out/* ./out_back
	@echo  -----build success------

android:
	mkdir android
	mkdir $(AND_JNI)
	mkdir $(AND_JNIP)
	$(CP) Android.mk $(AND_JNI)
	$(CP) Application.mk $(AND_JNI)
	
	$(CP) $(LLP_PATH)$(N)*.c $(AND_JNIP)
	$(CP) $(LLP_PATH)$(N)*.h $(AND_JNIP)
	$(CP) lp_conf.c $(AND_JNIP)
	$(CP) lp_conf.h $(AND_JNIP)
	$(CP) llp.h     $(AND_JNIP)
	ndk-build -C.$(N)$(AND_JNI)$(N)

LP: BUILD_PATH $(_LP)

LLP: BUILD_PATH $(_LLP)

BUILD_PATH:
	mkdir $(LLP_OUT) 	 
	

	
$(BUILD_LLP_O) : 
	$(CC) $(CFLAGS) -c -o $@  -I.  $(basename $@).c
	
$(BUILD_LP_O) : 
	$(CC) $(CFLAGS) -c -o $@  -I. $(basename $@).c

$(BUILD_LLP_JO): 
	$(CC) $(CFLAGS) -c -o $@ -I. $(JNI) $(basename $@).c

$(LP_CONF_O): 
	$(CC) $(CFLAGS) -c -o $(LP_CONF_O) -I. -I.. $(basename $(LP_CONF_O)).c

$(_LLP): $(LP_CONF_O) $(BUILD_LLP_O)
	$(AR) $(_LLP) $?
	
$(_LLPJAVA):  $(_LLP) $(BUILD_LLP_JO)
	$(CC) -shared  $(DLLARG) -o $(_LLPJAVA)  $? $(_LLP)

$(_LP): $(LP_CONF_O) $(BUILD_LP_O)
	$(CC) -o $@ $?

	
OD  = $(BUILD_LLP_O) $(BUILD_LLP_JO) $(BUILD_LP_O) $(LP_CONF_O) 
OD := $(OD) $(foreach s, $(OD), $(basename $(s)).d)

.PHONY : clean
clean:
	$(RM)  $(OD)
	$(RMDIR) $(LLP_OUT)
	$(RMDIR)  android
