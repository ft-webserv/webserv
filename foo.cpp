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
    atexit(leakCheck);
    foo *me = new foo();
    me->deletme();
    // delete me;
    // int i = 0;
    // char buf[30];
    // std::string tmp = "asdf 123123 asdf";
    // std::stringstream ss(tmp);

    // while (1)
    // {
    //     ss.read(buf, 5);
    //     std::cout << buf << std::endl;
    //     if (ss.eof() == true)
    //         break;
    // }
}
