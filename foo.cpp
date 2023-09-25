#include <iostream>
#include <map>
#include <string>
#include <sstream>

int main()
{
    std::string tmp = "301";
    std::string tmp1 = "adfasdfasdf";
    std::stringstream ss;
    int sc;
    std::string s;

    ss << tmp;
    ss << tmp1;
    ss >> sc >> s;
    std::cout << sc << " " << s << std::endl;
    return 0;
}
