#include "NodeMemoryModifier.h"
#include "ConsoleLogger.h"

NodeMemoryModifier::NodeMemoryModifier(DWORD processId) : hProcess(NULL), pid(processId) {};

NodeMemoryModifier::~NodeMemoryModifier() {
    if (hProcess) CloseHandle(hProcess);
};

bool NodeMemoryModifier::OpenTargetProcess(){
    hProcess = OpenProcess(
        PROCESS_VM_READ |
        PROCESS_VM_WRITE |
        PROCESS_VM_OPERATION |
        PROCESS_QUERY_INFORMATION |
        PROCESS_TERMINATE,
        FALSE,
        pid
    );

    if (hProcess == NULL) {
        ConsoleLogger::LOG_PROCESS_NOT_OPENED();
        throw std::logic_error("hProcess == NULL");
        return false;
    }

    ConsoleLogger::LOG_PROCESS_OPENED();
    return true;
}

void NodeMemoryModifier::ListProcessModules() {
    HMODULE modules[1024];
    DWORD needed;

    if (EnumProcessModules(hProcess, modules, sizeof(modules), &needed)) {
        std::cout << "\n=== Process Modules ===" << std::endl;
        for (unsigned int i = 0; i < (needed / sizeof(HMODULE)); i++) {
            char moduleName[MAX_PATH];
            if (GetModuleFileNameExA(hProcess, modules[i], moduleName, sizeof(moduleName))) {
                std::cout << "[" << i << "] " << moduleName << std::endl;
            }
        }
        std::cout << "=======================\n" << std::endl;
    }
}

std::vector<LPVOID> NodeMemoryModifier::FindValueInMemory(int targetValue) {
    std::vector<LPVOID> addresses;
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    LPVOID startAddress = sysInfo.lpMinimumApplicationAddress;
    LPVOID endAddress = sysInfo.lpMaximumApplicationAddress;

    MEMORY_BASIC_INFORMATION memInfo;
    int foundCount = 0;

    for (LPVOID address = startAddress;
        address < endAddress && foundCount < 10;
        address = (LPVOID)((char*)address + memInfo.RegionSize)) {

        if (VirtualQueryEx(hProcess, address, &memInfo, sizeof(memInfo))) {
            if (memInfo.State == MEM_COMMIT &&
                (memInfo.Protect & PAGE_READWRITE) &&
                !(memInfo.Protect & PAGE_GUARD)) {

                std::vector<char> buffer(memInfo.RegionSize);
                SIZE_T bytesRead;

                if (ReadProcessMemory(hProcess, memInfo.BaseAddress,
                    buffer.data(), memInfo.RegionSize, &bytesRead)) {

                    for (size_t i = 0; i <= bytesRead - sizeof(int); i++) {
                        int value = *(int*)(buffer.data() + i);
                        if (value == targetValue) {
                            LPVOID foundAddress = (LPVOID)((char*)memInfo.BaseAddress + i);
                            addresses.push_back(foundAddress);
                           
                            ConsoleLogger::LOG_ADDRESS_AND_VALUE_FOUND(foundAddress, value);
                            
                            foundCount++;
                            if (foundCount >= 10) break;
                        }
                    }
                }
            }
        }
    }

    return addresses;
}

bool NodeMemoryModifier::WriteToMemory(LPVOID address, int newValue) {
    if (!hProcess) {
        std::cerr << "[ERROR] Process not opened" << std::endl;
        return false;
    }

    int currentValue;
    SIZE_T bytesRead;

    if (ReadProcessMemory(hProcess, address, &currentValue, sizeof(currentValue), &bytesRead)) {
        ConsoleLogger::LOG_ADDRESS_AND_VALUE_FOUND(address, currentValue);
    }

    SIZE_T bytesWritten;
    BOOL result = WriteProcessMemory(hProcess, address, &newValue, sizeof(newValue), &bytesWritten);

    if (result && bytesWritten == sizeof(newValue)) {
        ConsoleLogger::LOG_SUCCESS_WROTE(newValue, address);

        int verifiedValue;
        if (ReadProcessMemory(hProcess, address, &verifiedValue, sizeof(verifiedValue), &bytesRead)) {
            return verifiedValue == newValue;
        }
    }
    else {
        ConsoleLogger::LOG_FAIL_WROTE();
    }

    return false;
}

bool NodeMemoryModifier::FindAndModifyNodeBuffer(double targetValue, double newValue) {

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    LPVOID startAddress = sysInfo.lpMinimumApplicationAddress;
    LPVOID endAddress = sysInfo.lpMaximumApplicationAddress;

    MEMORY_BASIC_INFORMATION memInfo;
    int foundBuffers = 0;

    for (LPVOID address = startAddress; address < endAddress;
        address = (LPVOID)((char*)address + memInfo.RegionSize)) {

        if (VirtualQueryEx(hProcess, address, &memInfo, sizeof(memInfo))) {
            if (memInfo.State == MEM_COMMIT &&
                (memInfo.Protect & PAGE_READWRITE)) {

                std::vector<char> buffer(memInfo.RegionSize);
                SIZE_T bytesRead;

                if (ReadProcessMemory(hProcess, memInfo.BaseAddress,
                    buffer.data(), memInfo.RegionSize, &bytesRead)) {

                    for (size_t i = 0; i <= bytesRead - sizeof(double); i++) {
                        double value = *(double*)(buffer.data() + i);

                        if (abs(value - targetValue) < 0.0001) {
                            LPVOID foundAddress = (LPVOID)((char*)memInfo.BaseAddress + i);
                            ConsoleLogger::LOG_ADDRESS_AND_VALUE_FOUND(foundAddress, value);

                            SIZE_T bytesWritten;
                            WriteProcessMemory(hProcess, foundAddress, &newValue,
                                sizeof(newValue), &bytesWritten);

                            foundBuffers++;

                            if (foundBuffers >= 5) return true;
                        }
                    }
                }
            }
        }
    }

    return foundBuffers > 0;
};

std::vector<LPVOID> NodeMemoryModifier::PatternScan(const std::vector<BYTE>& pattern, const std::string& mask) {
    std::vector<LPVOID> results;
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    LPVOID startAddress = sysInfo.lpMinimumApplicationAddress;
    LPVOID endAddress = sysInfo.lpMaximumApplicationAddress;

    MEMORY_BASIC_INFORMATION memInfo;

    for (LPVOID address = startAddress; address < endAddress;
        address = (LPVOID)((char*)address + memInfo.RegionSize)) {

        if (VirtualQueryEx(hProcess, address, &memInfo, sizeof(memInfo))) {
            if (memInfo.State == MEM_COMMIT &&
                (memInfo.Protect & PAGE_READWRITE)) {

                std::vector<BYTE> buffer(memInfo.RegionSize);
                SIZE_T bytesRead;

                if (ReadProcessMemory(hProcess, memInfo.BaseAddress,
                    buffer.data(), memInfo.RegionSize, &bytesRead)) {

                    for (size_t i = 0; i <= bytesRead - pattern.size(); i++) {
                        bool found = true;

                        for (size_t j = 0; j < pattern.size(); j++) {
                            if (mask[j] == 'x' && buffer[i + j] != pattern[j]) {
                                found = false;
                                break;
                            }
                        }

                        if (found) {
                            LPVOID foundAddress = (LPVOID)((char*)memInfo.BaseAddress + i);
                            results.push_back(foundAddress);
                        }
                    }
                }
            }
        }
    }

    return results;
};
