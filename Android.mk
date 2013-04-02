LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libllpjava

# compile in ARM mode, since the glyph loader/renderer is a hotspot
# when loading complex pages in the browser
#


LOCAL_ARM_MODE := arm

LOCAL_CFLAGS += -Wall
LOCAL_CFLAGS += -fPIC -DPIC
LOCAL_CFLAGS += "-DDARWIN_NO_CARBON"
LOCAL_CFLAGS += "-DFT2_BUILD_LIBRARY"

# the following is for testing only, and should not be used in final builds
# of the product
#LOCAL_CFLAGS += "-DTT_CONFIG_OPTION_BYTECODE_INTERPRETER"

LOCAL_CFLAGS += -O2

LOCAL_SRC_FILES := \
	src/lib_al.c \
	src/lib_io.c \
	src/lib_lp.c \
	src/lib_mes.c \
	src/lib_stringpool.c \
	src/lib_table.c \
	src/llpjava.c \
	src/lp_conf.c \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/src $(LOCAL_PATH)


# enable the FreeType internal memory debugger in the simulator
# you need to define the FT2_DEBUG_MEMORY environment variable
# when running the program to activate it. It will dump memory
# statistics when FT_Done_FreeType is called
#
ifeq ($(TARGET_SIMULATOR),true)
LOCAL_CFLAGS += "-DFT_DEBUG_MEMORY"
endif
                    
LOCAL_LDLIBS    := -ldl -lstdc++

include $(BUILD_SHARED_LIBRARY)