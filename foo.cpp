#include <string>
#include <iostream>

int main()
{
	std::string foo = "";

	foo.append("asdfasdf");
	foo.append("World");
	std::cout << foo << std::endl;
	return (0);
}
