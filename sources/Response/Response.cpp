#include "Response.hpp"
#include "Utils.hpp"

Response::Response()
	: _serverInfo(NULL), _locationInfo(NULL)
{
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
}

Response::~Response()
{
}

void Response::initResponse()
{
	_headerFields.clear();
	_body.clear();
	_response.clear();
	_serverInfo = NULL;
	_locationInfo = NULL;
}

std::string &Response::getResponse() { return (_response); };
ServerInfo *Response::getServerInfo() { return (_serverInfo); }
LocationInfo *Response::getLocationInfo() { return (_locationInfo); }
void Response::setServerInfo(ServerInfo *serverBlock) { _serverInfo = serverBlock; }
void Response::setLocationInfo(LocationInfo *locationBlock)
{
	_locationInfo = locationBlock;
	std::map<std::string, std::string> tmp = _locationInfo->getLocationInfo();
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
}

void Response::handleGet(Request &rqs)
{
	_statusCode = _200_OK;
	std::string location = rqs.getParsedRequest()._location;
	std::string root;

	root = _findRoot();
	_getFile(root, location);
	_makeResponse();
}

void Response::handlePost(Request &rqs)
{
	_statusCode = _201_CREATED;
	std::string location = rqs.getParsedRequest()._location;
	std::string root;

	root = _findRoot();
	_postFile(root, location, rqs);
	_makeResponse();
}

void Response::handleDelete(Request &rqs)
{
	_statusCode = _204_NO_CONTENT;
	std::string location = rqs.getParsedRequest()._location;
	std::string root;

	root = _findRoot();
	_deleteFile(root, location);
	_makeResponse();
}

void Response::handleCgi(Request &rqs)
{
	std::string path;

	path = _findRoot() + _cgi.cgiPath;
	if (access(path.c_str(), F_OK) == -1)
		throw(_404_NOT_FOUND);
	_makeEnvList();
}

std::string Response::_findRoot()
{
	std::string root;
	std::map<std::string, std::string> tmp;
	std::map<std::string, std::string>::iterator it;

	tmp = _serverInfo->getServerInfo();
	root = mapFind(tmp, "root");
	if (_locationInfo != NULL)
	{
		tmp = _locationInfo->getLocationInfo();
		root = mapFind(tmp, "root");
	}
	return (root);
}

void Response::_getFile(std::string root, std::string location)
{
	std::string path;
	struct stat buf;

	path = _makePath(root, location);
	if (_locationInfo->getPath() == location)
	{
		std::map<std::string, std::string> tmp = _locationInfo->getLocationInfo();
		std::map<std::string, std::string>::iterator it;

		it = tmp.begin();
		for (; it != tmp.end(); it++)
		{
			if (it->second == "index")
			{
				std::string indexPath;
				indexPath = path + "/" + it->first;
				if (stat(indexPath.c_str(), &buf) != -1)
				{
					_setResponse(indexPath, buf.st_size);
					return;
				}
			}
		}
		if (_isAutoIndex() == true)
		{
			_showFileList(path);
		}
		else
			throw(_404_NOT_FOUND);
	}
	/////찢어야함.
	else
	{
		if (stat(path.c_str(), &buf) == -1)
			throw(_404_NOT_FOUND);
		switch (buf.st_mode & S_IFMT)
		{
		case S_IFREG:
			_setResponse(path, buf.st_size);
			break;
		case S_IFDIR:
			if (_isAutoIndex() == true)
			{
				_showFileList(path);
			}
			else
				throw(_404_NOT_FOUND);
			break;
		default:
			break;
		}
	}
}

void Response::_postFile(std::string root, std::string location, Request &rqs)
{
	std::string path;
	struct stat buf;

	path = _makePath(root, location);
	if (stat(path.c_str(), &buf) == -1)
		throw(_404_NOT_FOUND);
	switch (buf.st_mode & S_IFMT)
	{
	case S_IFREG:
		throw(_400_BAD_REQUEST);
	case S_IFDIR:
		_createFile(path, location, rqs);
		break;
	default:
		break;
	}
}

void Response::_deleteFile(std::string root, std::string location)
{
	std::string path;
	struct stat buf;

	path = _makePath(root, location);
	if (stat(path.c_str(), &buf) == -1)
		throw(_404_NOT_FOUND);
	if (buf.st_mode & S_IFDIR)
		throw(_403_FORBIDDEN);
	if (std::remove(path.c_str()) != 0)
		throw(_500_INTERNAL_SERVER_ERROR);
	// _headerFields.insert(std::pair<std::string, std::string>("Content-Length:", "19"));
	// _headerFields.insert(std::pair<std::string, std::string>("Content-Type:", "application/json"));
	// _body += "\r\n{\"success\":\"true\"}";
}

std::string Response::_makePath(std::string root, std::string location)
{
	std::string path;

	path = "." + root + location;
	for (std::string::size_type i = path.find("//"); i != std::string::npos; i = path.find("//"))
		path.erase(i + 1, 1);
	if (*path.rbegin() == '/')
		path.pop_back();
	return (path);
}

void Response::_setResponse(std::string path, off_t size)
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
		pos += 1;
		std::string extension = path.substr(pos, path.length() - pos);
		_headerFields.insert(std::pair<std::string, std::string>("Content-Type:", conf.getMimeType().find(extension)->second));
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
		Exception::fileOpenError("file open error!");
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
	_response += "\r\n" + _body;
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
			_body += "<td><a href=\"" + name + "\">" + "📄 " + name + "</a></td>";
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
	std::map<std::string, std::string> tmp = _locationInfo->getLocationInfo();
	std::map<std::string, std::string>::iterator it;
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

bool Response::isCgi()
{
	std::map<std::string, std::string> tmp = _locationInfo->getLocationInfo();

	if (mapFind(tmp, "cgi_exec") == "" || mapFind(tmp, "cgi_path") == "")
		return (false);
	_cgi.cgiExec = mapFind(tmp, "cgi_exec");
	_cgi.cgiPath = mapFind(tmp, "cgi_path");
}

std::string Response::getErrorPage()
{
	std::string findResult;
	std::stringstream ss;

	if (_serverInfo != NULL)
	{
		std::map<std::string, std::string> tmp = _serverInfo->getServerInfo();
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

void Response::_createFile(std::string path, std::string location, Request &rqs)
{
	Config &conf = Config::getInstance();
	std::string fileName;
	std::ofstream os;

	fileName = _makeRandomName() + "." + mapFind(conf.getMimeType(), rqs.getParsedRequest()._contentType);
	os.open(path + "/" + fileName);
	if (os.is_open() == false)
		throw(_500_INTERNAL_SERVER_ERROR);
	os.write(rqs.getParsedRequest()._body.c_str(), rqs.getParsedRequest()._body.size());
	os.close();
	if (*location.rbegin() != '/')
		_headerFields.insert(std::pair<std::string, std::string>("Location:", location + "/" + fileName));
	else
		_headerFields.insert(std::pair<std::string, std::string>("Location:", location + fileName));
	_headerFields.insert(std::pair<std::string, std::string>("Content-Length:", "19"));
	_headerFields.insert(std::pair<std::string, std::string>("Content-Type:", "application/json"));
	_body += "\r\n{\"success\":\"true\"}";
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

void Response::_makeEnvList()
{
}

void _addEnv(std::string key, std::string value)
{
	std::string tmp;

	tmp = key + "=" + value;
}
