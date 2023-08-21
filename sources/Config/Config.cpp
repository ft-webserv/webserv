#include "config.hpp"

Config::Config()
{
}

Config::~Config()
{
}

Config::Config(const Config& source)
{
	(void)source;
}

Config*	Config::getConfFile()
{
	if (confFile == NULL)
		confFile = new Config();
	return (confFile);
}

void	Config::openFile(char *fileName)
{
	t_fileInfo	fileInfo;

	fileInfo.file.open(fileName);
	if (!fileInfo.file.is_open())
		throw FileError();
	fileInfo.file.seekg(0, std::ios::end);
	fileInfo.bufSize = fileInfo.file.tellg();
	fileInfo.buf.resize(fileInfo.bufSize);
	fileInfo.file.seekg(0, std::ios::beg);
	fileInfo.file.read(&fileInfo.buf[0], fileInfo.bufSize);
	std::cout << fileInfo.buf << std::endl;
}

void	Config::setFile(char *fileName)
{
	openFile(fileName);
}

const char *Config::FileError::what() const throw()
{
	return ("File open error!");
}

// class Config
// {
// private:
// 	static Config						conf_file;
// 	std::map<std::string, std::string>	conf_parse;
// 	Config();
// 	Config(const Config& source);
// 	~Config();
// 	void	parsing_config(std::string file);
// public:
// 	void								set_file();
// 	std::map<std::string, std::string>	get_file();
// };
