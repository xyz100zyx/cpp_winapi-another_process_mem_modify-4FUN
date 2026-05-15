#include <iostream>
#include "ConsoleLogger.h"

void ConsoleLogger::LOG_PROCESS_OPENED() {
	std::cout << "Процесс открыт ;)" << std::endl;
}

void ConsoleLogger::LOG_PROCESS_NOT_OPENED() {
	std::cerr << "Не удалось открыть процесс ;(" << std::endl;
}

void ConsoleLogger::LOG_ADDRESS_AND_VALUE_FOUND(const LPVOID& addr, int value) {
	std::cout << "Адрес = " << addr<< ", значение = " << value << std::endl;
}

void ConsoleLogger::LOG_NOT_OPENED_PROCESS_ERROR() {
	std::cerr << "Процесс закрыт" << std::endl;
}

void ConsoleLogger::LOG_SUCCESS_WROTE(int val, const LPVOID& addr) {
	std::cerr << "Успешно записано значение - адрес " << val << " в адрес = " << addr << std::endl;
}

void ConsoleLogger::LOG_FAIL_WROTE() {
	std::cerr << "Ошибка записи в адрес: " << GetLastError() << std::endl;
}

void ConsoleLogger::FINAL_SUCCESS() {
	std::cout << "Успешно записано" << std::endl;
}

//18240