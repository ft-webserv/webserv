#include "Response.hpp"

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
	std::map<std::string, std::string> tmp;
	std::map<std::string, std::string>::iterator it;

	tmp = _serverInfo->getServerInfo();
	root = mapFind(tmp, "root");
	if (_locationInfo != NULL)
	{
		tmp = _locationInfo->getLocationInfo();
		if (tmp.find("GET") == tmp.end())
			throw(_405_METHOD_NOT_ALLOWED);
		root = mapFind(tmp, "root");
	}
	if (root[root.length() - 1] == '/')
		root.pop_back();
	if (location[location.length() - 1] == '/')
		location.pop_back();
	_findFile(root, location);
	_makeResponse();
}

void Response::handlePost(Request &rqs)
{
	_statusCode = _201_CREATED;
}

void Response::handleDelete(Request &rqs)
{
	_statusCode = _204_NO_CONTENT;
}

void Response::_findFile(std::string root, std::string location)
{
	std::string path;
	struct stat buf;

	if (_locationInfo->getPath() == location)
	{
		std::map<std::string, std::string> tmp = _locationInfo->getLocationInfo();
		std::map<std::string, std::string>::iterator it;
		it = tmp.begin();
		for (; it != tmp.end(); it++)
		{
			if (it->second == "index")
			{
				path = "." + root + location + "/" + it->first;
				if (stat(path.c_str(), &buf) != -1)
				{
					_setResponse(path, buf.st_size);
					return;
				}
			}
		}
		if (_isAutoIndex() == true)
		{
			_showFileList(path, location);
		}
		else
			throw(_404_NOT_FOUND);
	}
	/////찢어야함.
	else
	{
		path = "." + root + location;
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
				_showFileList(path, location);
			}
			else
				throw(_404_NOT_FOUND);
			break;
		default:
			break;
		}
	}
}

void Response::_setResponse(std::string path, off_t size)
{
	Config &conf = Config::getInstance();
	std::string::size_type pos;
	std::stringstream length;

	//_setHeader로 빼기.
	length << size;
	_headerFields.insert(std::pair<std::string, std::string>("Conent-Length:", length.str()));
	pos = path.rfind(".");
	if (pos == std::string::npos)
		_headerFields.insert(std::pair<std::string, std::string>("Conent-Type:", "application/octet-stream"));
	else
	{
		pos += 1;
		std::string extension = path.substr(pos, path.length() - pos);
		_headerFields.insert(std::pair<std::string, std::string>("Conent-Type:", conf.getMimeType().find(extension)->second));
	}
	_setBody(path, size);
}

void Response::_setBody(std::string path, off_t size)
{
	std::ifstream file(path.c_str());

	_body.resize(size);
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
	_response = "HTTP/1.1 " + status.str() + _statusText[type][detail];
	for (it = _headerFields.begin(); it != _headerFields.end(); it++)
		_response += it->first + " " + it->second + "\r\n";
	_response += "\r\n" + _body;
}

void Response::_showFileList(std::string path, std::string location)
{
	if (path[path.length() - 1] != '/')
		path[path.length() - 1] += '/';
}

bool Response::_isAutoIndex()
{
	std::map<std::string, std::string> tmp = _locationInfo->getLocationInfo();
	std::map<std::string, std::string>::iterator it;
	if (mapFind(tmp, "autoindex") == "on")
		return (true);
	return (false);
}
