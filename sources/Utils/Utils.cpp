#include "Utils.hpp"

std::string mapFind(std::map<std::string, std::string> tmp, std::string word)
{
	std::map<std::string, std::string>::iterator it;

	it = tmp.begin();
	for (; it != tmp.end(); it++)
		if (it->second == word)
			return (it->first);
	return ("");
}
