#include "LocationInfo.hpp"

LocationInfo::LocationInfo(void)
{
}

LocationInfo::LocationInfo(const LocationInfo &src)
{
	*this = src;
}

LocationInfo::~LocationInfo(void)
{
}

LocationInfo &LocationInfo::operator=(LocationInfo const &rhs)
{
	if (this != &rhs)
	{
		this->_path = rhs._path;
		this->_info = rhs._info;
	}
	return *this;
}

void LocationInfo::setPath(std::string path)
{
	_path = path;
}

void LocationInfo::setLocationInfo(std::pair<std::string, std::string> pair)
{
	_info.insert(pair);
}

std::map<std::string, std::string> &LocationInfo::getLocationInfo()
{
	return (_info);
}

std::string &LocationInfo::getPath()
{
	return (_path);
}

void LocationInfo::printLocationInfo()
{
	std::cout << _path << " " << std::endl;
	std::map<std::string, std::string>::iterator it;
	it = _info.begin();
	for (; it != _info.end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;
}
