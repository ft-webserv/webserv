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
	std::string word, key, val;
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
		else if (word == "include")
			parseInclude(file);
		else
		{
			val = word;
			while (true)
			{
				file >> word;
				key = word;
				key.erase(std::find(key.begin(), key.end(), ';'));
				_generalInfo.insert(std::pair<std::string, std::string>(key, val));
				if (std::find(word.begin(), word.end(), ';') != word.end())
					break;
			}
		}
		file >> word;
	}
	// for (auto i = _mimeType.begin(); i != _mimeType.end(); i++)
	// 	std::cout << i->first << "     " << i->second << std::endl;
	for (int i = 0; i < _serverInfos.size(); i++)
		_serverInfos[i]->getServerInfo();
}

void Config::parseServerInfo(std::ifstream &file)
{
	int flag = 0;
	ServerInfo tmp;
	std::string word, key, val;

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
		{
			val = word;
			while (true)
			{
				file >> word;
				key = word;
				key.erase(std::find(key.begin(), key.end(), ';'));
				if (val == "listen")
				{
					std::stringstream ss;
					port_t port;
					ss << key;
					ss >> port;
					_ports.insert(port);
				}
				tmp.setServerInfo(std::pair<std::string, std::string>(key, val));
				if (std::find(word.begin(), word.end(), ';') != word.end())
					break;
			}
		}
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
	std::string word, key, val;

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
		{
			val = word;
			while (true)
			{
				file >> word;
				key = word;
				key.erase(std::find(key.begin(), key.end(), ';'));
				tmp.setLocationInfo(std::pair<std::string, std::string>(key, val));
				if (std::find(word.begin(), word.end(), ';') != word.end())
					break;
			}
		}
		if (flag == 0)
		{
			target.addLocationInfo(new LocationInfo(tmp));
			return;
		}
	}
}

void Config::parseInclude(std::ifstream &file)
{
	std::string word, key, val;
	std::ifstream mimeFile;

	file >> word;
	word.erase(std::find(word.begin(), word.end(), ';'));
	if (word.find("mime.types") == std::string::npos)
		return;
	mimeFile.open(word);
	if (mimeFile.is_open() == false)
		throw FileOpenFailException();
	mimeFile >> val;
	while (true)
	{
		if (mimeFile.eof() == true)
			break;
		while (true)
		{
			mimeFile >> key;
			_mimeType.insert(std::pair<std::string, std::string>(key, val));
			if (std::find(key.begin(), key.end(), ';') != key.end())
			{
				key.erase(std::find(key.begin(), key.end(), ';'));
				_mimeType.insert(std::pair<std::string, std::string>(key, val));
				break;
			}
		}
		mimeFile >> val;
	}
}

std::vector<ServerInfo *> &Config::getServerInfos()
{
	return (_serverInfos);
}

std::map<std::string, std::string> &Config::getgeneralInfo()
{
	return (_generalInfo);
}

std::set<int> &Config::getPorts(void)
{
	return (_ports);
}

const char *Config::FileOpenFailException::what() const throw()
{
	return ("file open error");
}
