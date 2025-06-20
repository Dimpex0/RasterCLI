#include <iostream>
#include <string>
#include "SessionCommandProcessor.h"
#include "Image.h"

int main()
{
    // !!!! При команда save, снимките се запазват в директорията, от която са прочетени !!!!

    //std::ifstream test1("tests/test1/test1.txt");
    //SessionCommandProcessor processor;
    //processor.run(test1);
    //test1.close();

    //std::ifstream test2("tests/test2/test2.txt");
    //SessionCommandProcessor processor;
    //processor.run(test2);
    //test2.close();


    //std::ifstream test3("tests/test3/test3.txt");
    //SessionCommandProcessor processor;
    //processor.run(test3);
    //test3.close();

    SessionCommandProcessor processor;
    processor.run(std::cin);

    return 0;
}
