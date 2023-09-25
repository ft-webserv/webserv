#include <iostream>
#include <map>
#include <string>
#include <sstream>

int main()
{
    int s = 100;
    std::string tmp = "asdf ";

    tmp += s;
    std::cout << tmp << std::endl;
    return 0;
}
