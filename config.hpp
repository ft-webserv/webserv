#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>

class Config
{
private:
	static Config*						config_ptr;
	std::map<std::string, std::string>	conf_parse;
	Config();
	Config(const Config& source);
	~Config();
	void	parsing_config(std::string file);
public:
	void								set_file();
	std::map<std::string, std::string>	get_file();
};

#endif
