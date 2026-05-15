#include "StupidProcessValueWrite.h"
#include "ConsoleLogger.h"

#include <iostream>
#include <windows.h>
#include <string>

void WriteProcessMemoryStupid() {

    DWORD processId;
    LPVOID targetAddress;
    int changedValue;

    std::cout << "Введите process id: ";
    std::cin >> processId;

    std::cout << "Введите адресс в памяти переменной: ";
    std::cin >> std::hex >> targetAddress;

    std::cout << "Введите новое значение переменной: ";
    std::cin >> std::dec >> changedValue;

    HANDLE hProcess = OpenProcess(
        PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION,
        FALSE,
        processId
    );

    if (hProcess == NULL) {

        ConsoleLogger::LOG_PROCESS_NOT_OPENED();
        throw std::logic_error("hProcess == NULL");
    }

    ConsoleLogger::LOG_PROCESS_OPENED();

    int valueToChange;
    SIZE_T bytesRead;


    if (ReadProcessMemory(hProcess, targetAddress, &valueToChange, sizeof(valueToChange), &bytesRead)) {
        ConsoleLogger::LOG_VAL_BEFORE_CHANGE(valueToChange);
    }
    else {
        ConsoleLogger::CANT_READ_VAR_VAL_ON_MEM_ADDR();
    }

    SIZE_T bytesWritten;
    BOOL result = WriteProcessMemory(
        hProcess,
        targetAddress,
        &changedValue,
        sizeof(changedValue),
        &bytesWritten
    );

    if (result && bytesWritten == sizeof(changedValue)) {

        if (ReadProcessMemory(hProcess, targetAddress, &valueToChange, sizeof(valueToChange), &bytesRead)) {
            std::cout << "oil check : " << valueToChange << std::endl;
        }
    }
    else {
        std::cerr << "Не удалось записать в память значение (не прошла проверка значений):" << std::endl;
        throw std::logic_error((char*)GetLastError());
    }

    CloseHandle(hProcess);

    std::cout << std::endl;

}