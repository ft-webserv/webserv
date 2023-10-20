#pragma once

#include <map>
#include <string>
#include <sstream>
#include <cctype>

std::string mapFind(std::map<std::string, std::string>& tmp, std::string word);
size_t ft_stoi(std::string num);
std::string ft_itos(size_t num);
std::string findExtension(std::string fileName);
void ft_toupper(char& c);
