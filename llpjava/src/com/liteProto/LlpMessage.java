package com.liteProto;

public class LlpMessage {
	private long llpMesHandle;
	private String name;

	public LlpMessage(long handle, String name) {
		this.llpMesHandle = handle;
		this.name = name;
	}

	public LlpMessage(long handle, String name, byte[] buff) {
		this(handle, name);
		this.decode(buff);
	}

	// destory
	public void destory() {
		LlpJavaNative.llpMessageFree(llpMesHandle);
		llpMesHandle = 0;
	}

	// clear
	public void clear() {
		LlpJavaNative.llpMessageClr(llpMesHandle);
	}

	public void write(String fieldStr, int number) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		int ret = LlpJavaNative.llpWmesInteger(llpMesHandle, fieldStr, (long)number);
		if (ret == 0) {
			throw new RuntimeException("[LlpJavaNative WriteInt32]:  write message \"" + name + "\" field \"" + fieldStr + "\" number: " + number
					+ " is error.");
		}
	}

	public void write(String fieldStr, boolean bool) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		long number = bool ? 0 : 1;
		int ret = LlpJavaNative.llpWmesInteger(llpMesHandle, fieldStr, number);
		if (ret == 0) {
			throw new RuntimeException("[LlpJavaNative integer]:  write message \"" + name + "\" field \"" + fieldStr + "\" number: " + number
					+ " is error.");
		}
	}

	public void write(String fieldStr, long number) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		int ret = LlpJavaNative.llpWmesInteger(llpMesHandle, fieldStr, number);
		if (ret == 0) {
			throw new RuntimeException("[LlpJavaNative integer]:  write message \"" + name + "\" field \"" + fieldStr + "\" number: " + number
					+ " is error.");
		}
	}

	public void write(String fieldStr, float number) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		int ret = LlpJavaNative.llpWmesReal(llpMesHandle, fieldStr, (double)number);
		if (ret == 0) {
			throw new RuntimeException("[LlpJavaNative writeReal]:  write message \"" + name + "\" field \"" + fieldStr + "\" number: " + number
					+ " is error.");
		}
	}

	public void write(String fieldStr, double number) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		int ret = LlpJavaNative.llpWmesReal(llpMesHandle, fieldStr, number);
		if (ret == 0) {
			throw new RuntimeException("[LlpJavaNative writeReal]:  write message \"" + name + "\" field \"" + fieldStr + "\" number: " + number
					+ " is error.");
		}
	}

	public void write(String fieldStr, String str) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		if (str == null) {
			return;
		}

		int ret = LlpJavaNative.llpWmesString(llpMesHandle, fieldStr, str);
		if (ret == 0) {
			throw new RuntimeException("[LlpJavaNative WriteString]:  write message \"" + name + "\" field \"" + fieldStr + "\" str: " + str
					+ " is error.");
		}
	}

	public void write(String fieldStr, byte[] bytes) {
		if (bytes == null || fieldStr == null) {
			throw new NullPointerException();
		}

		int ret = LlpJavaNative.llpWmesBytes(llpMesHandle, fieldStr, bytes);
		if (ret == 0) {
			throw new RuntimeException("[LlpJavaNative writeBytes]:  write message \"" + name + "\" field \"" + fieldStr);
		}
	}

	public LlpMessage write(String fieldStr) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		long handle = LlpJavaNative.llpWmesMessage(llpMesHandle, fieldStr);
		if (handle == 0) {
			throw new RuntimeException("[LlpJavaNative writeMessage]:  write message \"" + name + "\" field \"" + fieldStr + "\"is error.");
		}

		return new LlpMessage(handle, fieldStr);
	}

	public long readInteger(String fieldStr, int alInx) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		return LlpJavaNative.llpRmesInteger(llpMesHandle, fieldStr, alInx);
	}

	public long readInteger(String fieldStr) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		return readInteger(fieldStr, 0);
	}

	public double readReal(String fieldStr, int alInx) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		return LlpJavaNative.llpRmesReal(llpMesHandle, fieldStr, alInx);
	}

	public double readReal(String fieldStr) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		return readReal(fieldStr, 0);
	}

	public String readString(String fieldStr, int alInx) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		byte[] str = LlpJavaNative.llpRmesString(llpMesHandle, fieldStr, alInx);

		return new String(str);
	}

	public String readString(String fieldStr) {
		return readString(fieldStr, 0);
	}

	public byte[] readBytes(String fieldStr, int alInx) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		byte[] ret = LlpJavaNative.llpRmesBytes(llpMesHandle, fieldStr, alInx);
		return ret;
	}

	public byte[] readBytes(String fieldStr) {
		return readBytes(fieldStr, 0);
	}

	public LlpMessage readMessage(String fieldStr, int alInx) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		long handle = LlpJavaNative.llpRmesMessage(llpMesHandle, fieldStr, alInx);
		if (handle == 0)
			return null;

		return new LlpMessage(handle, fieldStr);
	}

	public LlpMessage readMessage(String fieldStr) throws Exception {
		return readMessage(fieldStr, 0);
	}

	public int readSize(String fieldStr) {
		if (fieldStr == null) {
			throw new NullPointerException();
		}

		return LlpJavaNative.llpRmesSize(llpMesHandle, fieldStr);
	}

	// 编码
	public byte[] encode() {
		return LlpJavaNative.llpOutMessage(llpMesHandle);
	}

	// 解码
	public void decode(byte[] buff) {
		this.clear();
		if (LlpJavaNative.llpInMessage(buff, llpMesHandle) == 0) {
			throw new RuntimeException("[LlpJavaNative decode]:  decode message \"" + name + "\" is error.");
		}
	}

	public String getName() {
		return name;
	}

}
