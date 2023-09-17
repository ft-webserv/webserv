#include "LocationInfo.hpp"

LocationInfo::LocationInfo(void)
{
}

LocationInfo::LocationInfo(const LocationInfo &src)
{
	this->_path = src._path;
	this->_info = src._info;
}

LocationInfo::~LocationInfo(void)
{
}

LocationInfo &LocationInfo::operator=(LocationInfo const &rhs)
{
	if (this != &rhs)
	{
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

const std::map<std::string, std::string> LocationInfo::getLocationInfo() const
{
	// std::cout << _path << " " << std::endl;
	// std::map<std::string, std::string>::iterator it = _info.begin();
	// for (; it != _info.end(); it++)
	// {
	// 	std::cout << it->first << " : " << it->second << std::endl;
	// }
	return (_info);
}

const std::string LocationInfo::getPath() const
{
	return (_path);
}
