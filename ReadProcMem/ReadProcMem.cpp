#include <iostream>
#include <clocale>

#include "StupidProcessValueWrite.h"
#include "NodeMemoryModifier.h"

int main()
{

    setlocale(LC_ALL, "ru");

    // WriteProcessMemoryStupid();


    int result = nmm::modify();

    system("pause");
    return result;

}

