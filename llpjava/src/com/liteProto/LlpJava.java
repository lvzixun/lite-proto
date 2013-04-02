package com.liteProto;

import java.lang.Exception;

public class LlpJava {
	private long env;
	private static LlpJava instance = new LlpJava();

	LlpJava() {
		env = LlpJavaNative.llpNewEnv();
		if (env == 0) {
			throw new RuntimeException("[LlpJavaNative Env]: newEnv is error.");
		}
	}

	public static LlpJava instance() {
		return instance;
	}

	public void destory() {
		LlpJavaNative.llpFreeEnv(env);
	}

	public void regMessage(String[] regMsg) throws Exception {
		for (int i = 0; i < regMsg.length; i++) {
			this.regMessage(regMsg[i]);
		}
	}

	public void regMessage(String fileName) throws Exception {
		if (LlpJavaNative.llpRegMes(env, fileName) == 0) {
			throw new Exception("[LlpJavaNative RegMes]: regedit message \"" + fileName + "\" is error.");
		}
	}

	public void regMessage(byte[] fileData) throws Exception {
		if (LlpJavaNative.llpRegSMes(env, fileData) == 0) {
			throw new Exception("[LlpJavaNative RegMes]: regedit message from file data " + "is error.");
		}
	}

	public LlpMessage getMessage(String msg) throws Exception {
		long handle = LlpJavaNative.llpMessageNew(env, msg);
		if (handle == 0) {
			throw new Exception("[LlpJavaNative NewMes]: get message \"" + msg + "\" is error.");
		}

		LlpMessage llpMessage = new LlpMessage(handle, msg);
		return llpMessage;
	}

	public LlpMessage getMessage(String msg, byte[] buff) throws Exception {
		long handle = LlpJavaNative.llpMessageNew(env, msg);
		if (handle == 0 || buff == null) {
			throw new Exception("[LlpJavaNative NewMes]: get message \"" + msg + "\" is error.");
		}

		LlpMessage llpMessage = new LlpMessage(handle, msg, buff);
		return llpMessage;
	}
}
