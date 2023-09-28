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

size_t ft_stoi(std::string num)
{
	std::stringstream ss;
	size_t result;

	ss << num;
	ss >> result;

	return (result);
}

std::string ft_itos(size_t num)
{
	std::stringstream ss;

	ss << num;
	return (ss.str());
}
