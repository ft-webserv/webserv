#include "Config.hpp"

Config::Config()
{
}

Config::Config(const std::string &fileName)
{
	parseConfigFile(fileName);
}

Config::~Config()
{
}

Config::Config(const Config &source)
{
	(void)source;
}

void Config::parseConfigFile(const std::string &fileName)
{
	std::ifstream file;
	std::string word;
	std::string whitespace = " \t\r\n\f\v";

	file.open(fileName);

	if (file.is_open() == false)
		throw FileOpenFailException();

	file >> word;

	while (true)
	{
		if (file.eof() == true)
			break;
		if (word == "server")
			parseServerInfo(file);
		else
			_generalBlock.insert(getPair(file, word));
		file >> word;
	}
	for (int i = 0; i < _serverInfos.size(); i++)
		_serverInfos[i]->getServerInfo();
}

std::pair<std::string, std::string> Config::getPair(std::ifstream &file, std::string word)
{
	std::string value;
	std::string key;

	while (true)
	{
		if (std::find(word.begin(), word.end(), ';') != word.end())
		{
			value = word;
			return (std::pair<std::string, std::string>(key, value));
		}
		else
			key = word;
		file >> word;
	}
}

void Config::parseServerInfo(std::ifstream &file)
{
	int flag = 0;
	ServerInfo tmp;
	std::string word;

	while (true)
	{
		file >> word;
		if (word == "{")
			flag++;
		else if (word == "}")
			flag--;
		else if (word == "location")
			parseLocationInfo(file, tmp);
		else
			tmp.setServerInfo(getPair(file, word));
		if (flag == 0)
		{
			_serverInfos.push_back(new ServerInfo(tmp));
			return;
		}
	}
}

void Config::parseLocationInfo(std::ifstream &file, ServerInfo &target)
{
	int flag = 0;
	LocationInfo tmp;
	std::string word;

	file >> word;
	tmp.setPath(word);
	while (true)
	{
		file >> word;
		if (word == "{")
			flag++;
		else if (word == "}")
			flag--;
		else
			tmp.setLocationInfo(getPair(file, word));
		if (flag == 0)
		{
			target.addLocationInfo(new LocationInfo(tmp));
			return;
		}
	}
}

const char *Config::FileOpenFailException::what() const throw()
{
	return ("file open error");
}
