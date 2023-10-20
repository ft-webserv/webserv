#include "Response.hpp"
#include "Utils.hpp"

Response::Response()
	: _serverInfo(NULL), _locationInfo(NULL)
{
	Config &conf = Config::getInstance();

	_isHead = false;
	_statusText[0][0] = "OK";
	_statusText[0][1] = "CREATED";
	_statusText[0][2] = "ACCEPTED";
	_statusText[0][4] = "NO CONTENT";

	_statusText[1][1] = "MOVED PERMANENTLY";
	_statusText[1][2] = "FOUND";
	_statusText[1][3] = "SEE OTHER";
	_statusText[1][4] = "NOT MODIFIED";
	_statusText[1][7] = "TEMPORARY REDIRECT";
	_statusText[1][8] = "PERMANENT REDIRECT";
	_clientMaxBodySize = conf.getClientMaxBodySize();
}

Response::Response(const Response &src)
{
	*this = src;
}

Response &Response::operator=(Response const &rhs)
{
	if (this != &rhs)
	{
		this->_isHead = rhs._isHead;
		this->_statusCode = rhs._statusCode;
		this->_headerFields = rhs._headerFields;
		this->_body = rhs._body;
		this->_root = rhs._root;
		this->_path = rhs._path;
		this->_response = rhs._response;
		this->_statusText[0][0] = "OK";
		this->_statusText[0][1] = "CREATED";
		this->_statusText[0][2] = "ACCEPTED";
		this->_statusText[0][4] = "NO CONTENT";
		this->_statusText[1][1] = "MOVED PERMANENTLY";
		this->_statusText[1][2] = "FOUND";
		this->_statusText[1][3] = "SEE OTHER";
		this->_statusText[1][4] = "NOT MODIFIED";
		this->_statusText[1][7] = "TEMPORARY REDIRECT";
		this->_statusText[1][8] = "PERMANENT REDIRECT";
		this->_serverInfo = rhs._serverInfo;
		this->_locationInfo = rhs._locationInfo;
		this->_cgiInfo = rhs._cgiInfo;
		this->_clientMaxBodySize = rhs._clientMaxBodySize;
	}
	return *this;
}

Response::~Response()
{
}

void Response::initResponse()
{
	_isHead = false;
	_headerFields.clear();
	_body.clear();
	_root.clear();
	_path.clear();
	_response.clear();
	_serverInfo = NULL;
	_locationInfo = NULL;
	_cgiInfo.cgiExtension.clear();
	_cgiInfo.cgiInfo.clear();
}

const std::string &Response::getResponse() const { return (_response); }

std::string &Response::getRoot() { return (_root); }

std::string &Response::getPath(std::string location)
{
	_makePath(location);
	return (_path);
}

ServerInfo *Response::getServerInfo() { return (_serverInfo); }

LocationInfo *Response::getLocationInfo() { return (_locationInfo); }

bool &Response::getIsHead() { return (_isHead); }

t_cgiInfo &Response::getCgiInfo() { return (_cgiInfo); }

std::size_t &Response::getClientMaxBodySize() { return (_clientMaxBodySize); }

void Response::setServerInfo(ServerInfo *serverBlock)
{
	_serverInfo = serverBlock;
	std::size_t clientMaxSize = ft_stoi(mapFind(_serverInfo->getServerInfo(), "client_max_body_size"));
	if (clientMaxSize != 0)
	{
		_clientMaxBodySize = clientMaxSize;
	}
}

void Response::setLocationInfo(LocationInfo *locationBlock)
{
	_locationInfo = locationBlock;
	std::map<std::string, std::string> tmp = _locationInfo->getLocationInfo();

	std::size_t clientMaxSize = ft_stoi(mapFind(tmp, "client_max_body_size"));
	if (clientMaxSize != 0)
	{
		_clientMaxBodySize = clientMaxSize;
	}
	if (mapFind(tmp, "return").empty() == false)
	{
		std::stringstream ss;
		std::map<std::string, std::string>::iterator it = tmp.begin();
		std::string location;
		int statusCode;

		for (; it != tmp.end(); it++)
		{
			if (it->second == "return")
				ss << it->first;
		}
		ss >> statusCode >> location;
		_statusCode = static_cast<eStatus>(statusCode);
		_headerFields.insert(std::pair<std::string, std::string>("Location:", location));
		_makeResponse();
	}
	_findRoot();
}

void Response::handleGet(Request &rqs)
{
	_statusCode = _200_OK;
	std::string location = rqs.getParsedRequest()._location;

	_getFile(location);
	if (rqs.getParsedRequest()._method == "HEAD")
		_isHead = true;
	_makeResponse();
}

void Response::handlePost(Request &rqs)
{
	_statusCode = _201_CREATED;
	std::string location = rqs.getParsedRequest()._location;

	_postFile(location, rqs);
	_makeResponse();
}

void Response::handleDelete(Request &rqs)
{
	_statusCode = _204_NO_CONTENT;
	std::string location = rqs.getParsedRequest()._location;

	_deleteFile(location);
	_makeResponse();
}

void Response::_findRoot()
{
	std::string root;
	std::map<std::string, std::string> tmp;
	std::map<std::string, std::string>::iterator it;

	tmp = _serverInfo->getServerInfo();
	_root = mapFind(tmp, "root");
	if (_locationInfo != NULL)
	{
		tmp = _locationInfo->getLocationInfo();
		_root = mapFind(tmp, "root");
	}
}

void Response::_getFile(std::string location)
{
	struct stat buf;

	_makePath(location);
	if (stat(_path.c_str(), &buf) == -1)
		throw(_404_NOT_FOUND);
	switch (buf.st_mode & S_IFMT)
	{
	case S_IFREG:
		_setHeader(_path, buf.st_size);
		break;
	case S_IFDIR:
	{
		std::map<std::string, std::string> tmp = _locationInfo->getLocationInfo();
		std::map<std::string, std::string>::iterator it;

		it = tmp.begin();
		for (; it != tmp.end(); it++)
		{
			if (it->second == "index")
			{
				std::string indexPath;
				indexPath = _path + "/" + it->first;
				if (stat(indexPath.c_str(), &buf) != -1)
				{
					_setHeader(indexPath, buf.st_size);
					return;
				}
			}
		}
		if (_isAutoIndex() == true)
		{
			_showFileList(_path);
		}
		else
			throw(_404_NOT_FOUND);
		break;
	}
	default:
		break;
	}
}

void Response::_postFile(std::string location, Request &rqs)
{
	struct stat buf;

	_makePath(location);
	if (stat(_path.c_str(), &buf) == -1)
		throw(_404_NOT_FOUND);
	switch (buf.st_mode & S_IFMT)
	{
	case S_IFREG:
		throw(_400_BAD_REQUEST);
	case S_IFDIR:
		_createFile(location, rqs);
		break;
	default:
		break;
	}
}

void Response::_deleteFile(std::string location)
{
	struct stat buf;

	_makePath(location);
	if (stat(_path.c_str(), &buf) == -1)
		throw(_404_NOT_FOUND);
	if (buf.st_mode & S_IFDIR)
		throw(_403_FORBIDDEN);
	if (std::remove(_path.c_str()) != 0)
		throw(_500_INTERNAL_SERVER_ERROR);
	// _headerFields.insert(std::pair<std::string, std::string>("Content-Length:", "19"));
	// _headerFields.insert(std::pair<std::string, std::string>("Content-Type:", "application/json"));
	// _body += "\r\n{\"success\":\"true\"}";
}

void Response::_makePath(std::string location)
{
	std::string tmp;
	std::size_t pos;

	pos = location.find(_locationInfo->getPath());
	if (pos != std::string::npos && _locationInfo->getPath() != "/")
		location.erase(pos, _locationInfo->getPath().length());
	_path = "." + _root + location;
	for (std::string::size_type i = _path.find("//"); i != std::string::npos; i = _path.find("//"))
		_path.erase(i + 1, 1);
	if (*_path.rbegin() == '/')
		_path.pop_back();
}

void Response::_setHeader(std::string path, off_t size)
{
	Config &conf = Config::getInstance();
	std::string::size_type pos;
	std::stringstream length;

	//_setHeader로 빼기.
	length << size;
	_headerFields.insert(std::pair<std::string, std::string>("Content-Length:", length.str()));
	pos = path.rfind(".");
	if (pos == std::string::npos)
		_headerFields.insert(std::pair<std::string, std::string>("Content-Type:", "application/octet-stream"));
	else
	{
		std::string extension = findExtension(path);
		if (conf.getMimeType().find(extension) != conf.getMimeType().end())
			_headerFields.insert(std::pair<std::string, std::string>("Content-Type:", conf.getMimeType().find(extension)->second));
		else
			_headerFields.insert(std::pair<std::string, std::string>("Content-Type:", "application/octet-stream"));
	}
	// _headerFields.insert(std::pair<std::string, std::string>("Connection:", "keep-alive"));
	_setBody(path, size);
}

void Response::_setBody(std::string path, off_t size)
{
	std::ifstream file;

	_body.resize(size);
	file.open(path);
	if (file.is_open() == false)
		Exception::fileOpenError();
	file.read(&_body[0], size);
	file.close();
}

void Response::_makeResponse()
{
	std::stringstream status;
	std::map<std::string, std::string>::iterator it;
	int type = _statusCode / 100 - 2;
	int detail = _statusCode / 100 - 2;

	status << _statusCode;
	_response = "HTTP/1.1 " + status.str() + " " + _statusText[type][detail] + "\r\n";
	for (it = _headerFields.begin(); it != _headerFields.end(); it++)
		_response += it->first + " " + it->second + "\r\n";
	_response += "\r\n";
	if (_isHead == false)
		_response += _body;
}

void Response::_showFileList(std::string path)
{
	DIR *dirp = NULL;
	struct dirent *dp = NULL;
	struct stat buf;
	std::string name;
	std::string time;
	std::string dpPath;

	dirp = opendir(path.c_str());
	if (dirp == NULL)
	{
		throw(_500_INTERNAL_SERVER_ERROR);
	}
	_body += "<table width=\"450\" height=\"200\" align=\"left\">";
	_body += "<th>Name</th><th>Last Modified</th><th>Size</th>";
	while ((dp = readdir(dirp)) != NULL)
	{
		_body += "<tr>";
		name = dp->d_name;
		dpPath = path + "/" + name;
		stat(dpPath.c_str(), &buf);
		time = std::ctime(&buf.st_mtimespec.tv_sec);
		switch (buf.st_mode & S_IFMT)
		{
		case S_IFREG:
			_body += "<td><a href=\"" + dpPath + "\">" + "📄 " + name + "</a></td>";
			_body += "<td>" + time + "</td>";
			_body += "<td>" + ft_itos(buf.st_size) + "</td>";
			break;
		case S_IFDIR:
			_body += "<td><a href=\"" + name + "\">" + "📁 " + name + "/" + "</a></td>";
			_body += "<td>" + time + "</td>";
			_body += "<td>" + ft_itos(buf.st_size) + "</td>";
			break;
		default:
			break;
		}
		_body += "</tr>";
	}
	_body += "</table>";
	closedir(dirp);
	_headerFields.insert(std::pair<std::string, std::string>("Content-Length:", ft_itos(_body.length())));
	_headerFields.insert(std::pair<std::string, std::string>("Content-Type:", "text/html; charset=utf-8;"));
}

bool Response::_isAutoIndex()
{
	std::map<std::string, std::string> &tmp = _locationInfo->getLocationInfo();
	if (mapFind(tmp, "autoindex") == "on")
		return (true);
	return (false);
}

bool Response::isAllowedMethod(std::string method)
{
	std::map<std::string, std::string> tmp = _locationInfo->getLocationInfo();

	if (tmp.find(method) == tmp.end())
		return (false);
	return (true);
}

bool Response::isCgi(std::string location)
{
	std::map<std::string, std::string> tmp = _locationInfo->getLocationInfo();

	_cgiInfo.cgiInfo = mapFind(tmp, "cgi_info");
	if (_cgiInfo.cgiInfo == "")
	{
		tmp = _serverInfo->getServerInfo();
		_cgiInfo.cgiExtension = mapFind(tmp, "cgi_extension");
		_cgiInfo.cgiInfo = mapFind(tmp, "cgi_info");
		if (_cgiInfo.cgiExtension == "" || _cgiInfo.cgiInfo == "")
			return (false);
		if (findExtension(location) != _cgiInfo.cgiExtension)
			return (false);
	}
	return (true);
}

std::string Response::getErrorPage()
{
	std::string findResult;
	std::stringstream ss;

	if (_serverInfo != NULL)
	{
		std::map<std::string, std::string>& tmp = _serverInfo->getServerInfo();
		findResult = mapFind(tmp, "errorpage");
	}
	if (findResult.empty() == false)
		findResult = "." + findResult;
	if (findResult[findResult.length() - 1] == '/')
	{
		ss << _statusCode;
		findResult += ss.str() + ".html";
	}
	return (findResult);
}

void Response::_createFile(std::string location, Request &rqs)
{
	Config &conf = Config::getInstance();
	std::string fileName;
	std::ofstream os;

	fileName = _makeRandomName() + "." + mapFind(conf.getMimeType(), rqs.getParsedRequest()._contentType);
	os.open(_path + "/" + fileName);
	if (os.is_open() == false)
		throw(_500_INTERNAL_SERVER_ERROR);
	os.write(rqs.getParsedRequest()._body.c_str(), rqs.getParsedRequest()._body.size());
	os.close();
	if (*location.rbegin() != '/')
		_headerFields.insert(std::pair<std::string, std::string>("Location:", location + "/" + fileName));
	else
		_headerFields.insert(std::pair<std::string, std::string>("Location:", location + fileName));
	_headerFields.insert(std::pair<std::string, std::string>("Content-Length:", "18"));
	_headerFields.insert(std::pair<std::string, std::string>("Content-Type:", "application/json"));
	_body += "{\"success\":\"true\"}";
}

std::string Response::_makeRandomName()
{
	std::string format = "yyyymmddhhmmss";
	std::time_t time = std::time(NULL);
	std::string timeString;

	timeString.resize(format.size());
	std::strftime(&timeString[0], timeString.size(),
				  "%Y%m%d%H%M%S", std::localtime(&time));
	return (timeString);
}

void Response::setResponse(std::string &response)
{
	this->_response = response;
}
