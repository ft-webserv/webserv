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
    // atexit(leakCheck);
    // foo *me = new foo();
    // me->deletme();
    // delete me;
    int i = 0;
    char *buf;
    std::string tmp = "asdf 123123 asdf";
    std::stringstream ss(tmp);

    buf = new char[1000];
    while (1)
    {
        ss.clear();
        ss.read(buf, tmp.length() + 1);
        std::cout << buf << std::endl;
        if (ss.fail() == true)
        {
            std::cout << "fuck" << std::endl;
            break;
        }
        if (ss.eof() == true)
            break;
    }
}
