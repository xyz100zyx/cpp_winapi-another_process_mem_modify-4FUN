#ifndef NODE_PROC_MODIF

#define NODE_PROC_MODIF 2

#define NODE_INITIAL_VALUE 120265

#define WITH_LIST_ALL_PROCESS false

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <psapi.h>

#include "ConsoleLogger.h"

#pragma comment(lib, "psapi.lib")

class NodeMemoryModifier {
private:
    HANDLE hProcess;
    DWORD pid;

public:
    NodeMemoryModifier(DWORD processId);

    ~NodeMemoryModifier();

    // max perm
    bool OpenTargetProcess();


    void ListProcessModules();

    std::vector<LPVOID> FindValueInMemory(int targetValue);

    bool WriteToMemory(LPVOID address, int newValue);

    // strategy only for node buffer
    bool FindAndModifyNodeBuffer(double targetValue, double newValue);

    std::vector<LPVOID> PatternScan(const std::vector<BYTE>& pattern, const std::string& mask);
};
namespace nmm {
    
    inline int modify() {

        DWORD nodePID = 0;

        std::cout << "Введите node pid: ";
        std::cin >> nodePID;

        NodeMemoryModifier modifier(nodePID);

        if (!modifier.OpenTargetProcess()) {
            system("pause");
            return 1;
        }

#if WITH_LIST_ALL_PROCESS == true
        modifier.ListProcessModules();
#endif // WITH_LIST_ALL_PROCESS == true

        double currentValue = 120265;
            double newValue;

            std::cout << "New value: ";
            std::cin >> newValue;

            if (modifier.FindAndModifyNodeBuffer(currentValue, newValue)) {
                ConsoleLogger::FINAL_SUCCESS();
            }

        return 0;
    }

}

#endif
