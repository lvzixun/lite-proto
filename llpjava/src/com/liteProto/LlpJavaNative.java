package com.liteProto;

import pin.core.Pin;

public class LlpJavaNative {

	// --------new/free a env
	static native long llpNewEnv();

	static native void llpFreeEnv(long env);

	// --------regedit/delete .lpb files
	static native int llpRegMes(long env, String mesName);

	static native int llpRegSMes(long env, byte[] buff);

	// --------new/delete a message object
	static native long llpMessageNew(long env, String mesName);

	static native void llpMessageClr(long inMes);

	static native void llpMessageFree(long inMes);
	
	// -------get a message name
	static native byte[] llpMessageName(long inMes);

	// --------write a message object
	static native int llpWmesInteger(long lm, String fieldStr, long number);

	static native int llpWmesReal(long lm, String fieldStr, double number);

	static native int llpWmesString(long lm, String fieldStr, String str);

	static native int llpWmesBytes(long lm, String fieldStr, byte[] bytes);

	static native long llpWmesMessage(long lm, String fieldStr);

	// ---------read a message object
	static native long llpRmesInteger(long lm, String fieldStr, int alInx);
	
	static native double llpRmesReal(long lm, String fieldStr, int alInx);

	static native byte[] llpRmesString(long lm, String fieldStr, int alInx);

	static native byte[] llpRmesBytes(long lm, String fieldStr, int alInx);

	static native long llpRmesMessage(long lm, String fieldStr, int alInx);

	static native int llpRmesSize(long lm, String fieldStr);

	// ------- out/in a message body
	static native byte[] llpOutMessage(long lms);

	static native int llpInMessage(byte[] in, long lms);

//	static {
//		System.loadLibrary("llpjava");
//	}
//	
	static {
		Pin.loadLibrary("llpjava");
	}

	/*
	 * public static byte[] strByte(String str) { byte[] strData =
	 * str.getBytes(); byte[] destData = new byte[strData.length + 1];
	 * System.arraycopy(strData, 0, destData, 0, strData.length); return
	 * destData; }
	 */
}
