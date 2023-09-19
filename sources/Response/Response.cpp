#include "Response.hpp"

Response::Response()
	: _serverInfo(NULL), _locationInfo(NULL) {}

Response::~Response()
{
}

ServerInfo *Response::getServerInfo() { return (_serverInfo); }
LocationInfo *Response::getLocationInfo() { return (_locationInfo); }
void Response::setServerInfo(ServerInfo *serverBlock) { _serverInfo = serverBlock; }
void Response::setLocationInfo(LocationInfo *locationBlock) { _locationInfo = locationBlock; }

void Response::handleGet(Request &rqs)
{
	_statusCode = _200_OK;
	std::string root;
	std::map<std::string, std::string> tmp;
	std::map<std::string, std::string>::iterator it;

	tmp = _serverInfo->getServerInfo();
	it = tmp.begin();
	for (; it != tmp.end(); it++)
	{
		if (it->second == "root")
			root += it->first;
		else
			root += "/html";
	}
	if (_locationInfo != NULL)
	{
		tmp = _locationInfo->getLocationInfo();
		it = tmp.begin();
		for (; it != tmp.end(); it++)
		{
			if (it->second == "root")
				root = it->second;
			else
				root = rqs.getParsedRequest()._location;
		}
	}
	_findFile(root);
}

void Response::handlePost(Request &rqs)
{
	_statusCode = _201_CREATED;
}

void Response::handleDelete(Request &rqs)
{
	_statusCode = _204_NO_CONTENT;
}

void Response::_findFile(std::string path)
{
	struct stat buf;

	path = "." + path;
	if (stat(path.c_str(), &buf) == -1)
		_statusCode = _404_NOT_FOUND;
	switch (buf.st_mode & S_IFMT)
	{
	case S_IFREG:
		_setResponse(path, buf.st_size);
		break;
	case S_IFDIR:
		break;
	default:
		break;
	}
}

void Response::_setResponse(std::string path, off_t size)
{
	Config &conf = Config::getInstance();
	std::string::size_type pos;
	std::stringstream length;

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

std::string &Response::getResponse()
{
	std::stringstream status;
	std::map<std::string, std::string>::iterator it;

	status << _statusCode;
	_response = "HTTP/1.1 " + status.str() + " OK\r\n";
	for (it = _headerFields.begin(); it != _headerFields.end(); it++)
		_response += it->first + " " + it->second + "\r\n";
	_response += "\r\n" + _body;
	return (_response);
}
