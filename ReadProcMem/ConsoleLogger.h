#ifndef CNSL_LOGGER

#define CNSL_LOGGER 3

#include <windows.h>

class ConsoleLogger {

public:
	static void LOG_PROCESS_OPENED();

	static void LOG_PROCESS_NOT_OPENED();

	static void LOG_ADDRESS_AND_VALUE_FOUND(const LPVOID& addr, const int &value);

	static void LOG_NOT_OPENED_PROCESS_ERROR();

	static void LOG_SUCCESS_WROTE(const int &val, const LPVOID& addr);

	static void LOG_FAIL_WROTE();

	static void FINAL_SUCCESS();

	static void CANT_READ_VAR_VAL_ON_MEM_ADDR();

	static void LOG_VAL_BEFORE_CHANGE(const int &valueToChange);
};

#endif
