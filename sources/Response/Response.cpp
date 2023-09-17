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
	std::string root;
	std::map<std::string, std::string> tmp;
	std::map<std::string, std::string>::iterator it;

	tmp = _serverInfo->getServerInfo();
	it = tmp.find("root");
	if (it != tmp.end())
		root = it->second;
	else
		root = "/html";
	if (_locationInfo != NULL)
	{
		tmp = _locationInfo->getLocationInfo();
		it = tmp.find("root");
		if (it != tmp.end())
			root = it->second;
	}
	findFile(root + rqs.getParsedRequest()._location);
}

void Response::handlePost(Request &rqs)
{
}

void Response::handleDelete(Request &rqs)
{
}

void Response::findFile(std::string path)
{
	struct stat buf;

	if (stat(path.c_str(), &buf) == -1)
		throw(_statusCode = _404_NOT_FOUND);
	switch (buf.st_mode & S_IFMT)
	{
	case S_IFREG:
		_addContentType(path);
		break;
	case S_IFDIR:
		break;
	default:
		break;
	}
}

void Response::_addContentType(std::string path)
{
	Config &conf = Config::getInstance();
	std::string::size_type pos;

	pos = path.find(".");
	if (pos == std::string::npos)
		_headerFields.insert(std::pair<std::string, std::string>("Conent-Type", "application/octet-stream"));
	else
	{
		pos += 1;
		std::string extension = path.substr(pos, path.length() - pos);

		_headerFields.insert(std::pair<std::string, std::string>("Conent-Type", conf.getMimeType().find(extension)->second));
	}
}
