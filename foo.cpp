#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <ctime>

class foo
{
private:
public:
    void deletme()
    {
        delete this;
        std::cout << "hi" << std::endl;
    }
};

void leakCheck()
{
    system("leaks a.out");
}

int main()
{
    std::string foo("HELLO HELLO HI");
    // atexit(leakCheck);
    // foo *me = new foo();
    // me->deletme();
    // delete me;
    // int i = 0;
    // int *buf = new int[2];
    // int *&buf2 = buf;
    // delete[] buf;
    // buf = NULL;
    // std::cout << buf2 << std::endl;
    for (int i = 0; i < 100000000; i++)
        std::cout << "e";
    return (0);
}
