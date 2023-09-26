#include <iostream>
#include <map>
#include <string>
#include <sstream>

int main()
{
    int i = 0;
    std::string word;
    std::string tmp = "asdf 123123 asdf";
    std::istringstream ss(tmp);

    ss >> word;
    while (1)
    {
        i++;
        ss >> word;
        std::cout << word << i << std::endl;
        if (ss.eof() == true)
            break;
    }
    return 0;
}
