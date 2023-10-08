#include "Cgi.hpp"

Cgi::Cgi(Request *request, Response *response, uintptr_t socket)
{
	_request = request;
	_response = response;
	clientSock = socket;
	_envCnt = 0;
	_reqFd[0] = 0;
	_reqFd[1] = 0;
	_resFd[0] = 0;
	_resFd[1] = 0;
	_pid = 0;
	_body = request->getParsedRequest()._body;
	_cgiExec = mapFind(_response->getLocationInfo()->getLocationInfo(), "cgi_exec");
	_cgiPath = mapFind(_response->getLocationInfo()->getLocationInfo(), "cgi_path");
	_makeEnvList(clientSock);
}

void Cgi::_makeEnvList(uintptr_t clntSock)
{
	struct sockaddr_in clnt;
	socklen_t clntSockLen = sizeof(clnt);
	port_t port;

	getsockname(clntSock, (sockaddr *)&clnt, &clntSockLen);
	port = ntohs(clnt.sin_port);
	_addEnv("SERVER_SOFTWARE", "webserv/0.1");
	_addEnv("SERVER_PROTOCOL", "HTTP/1.1");
	_addEnv("SERVER_PORT", ft_itos(port));
	_addEnv("SERVER_NAME", mapFind(_response->getServerInfo()->getServerInfo(), "server_name"));
	_addEnv("GETWAY_INTERFACE", "CGI/1.1");
	_addEnv("REQUEST_METHOD", _request->getParsedRequest()._method);
	_addEnv("REMOTE_ADDR", ft_itos(clnt.sin_addr.s_addr));
	_addEnv("CONTENT_TYPE", _request->getParsedRequest()._contentType);
	_addEnv("CONTENT_LENGTH", _request->getParsedRequest()._contentLengthStr);
	_addEnv("SCRIPT_NAME", _cgiPath);
	_addEnv("PATH_INFO", _response->getRoot() + _request->getParsedRequest()._location);
	_cgiStart();
}

void Cgi::_addEnv(std::string key, std::string value)
{
	std::string tmp;

	tmp = key + "=" + value;
	_env[_envCnt] = new char[tmp.size() + 1];
	tmp.copy(_env[_envCnt], tmp.size());
	_env[_envCnt][tmp.size()] = '\0';
	_envCnt++;
	_env[_envCnt] = NULL;
}

void Cgi::_cgiStart()
{
	Kqueue &kqueue = Kqueue::getInstance();

	if (pipe(_reqFd) == -1)
		throw(_500_INTERNAL_SERVER_ERROR);
	if (pipe(_resFd) == -1)
		throw(_500_INTERNAL_SERVER_ERROR);
	_pid = fork();
	if (_pid == -1)
	{
		throw(_500_INTERNAL_SERVER_ERROR);
	}
	else if (_pid == 0)
	{
		char *args[3];

		dup2(_reqFd[0], STDIN_FILENO);
		dup2(_resFd[1], STDOUT_FILENO);
		close(_reqFd[0]);
		close(_resFd[1]);
		close(_reqFd[1]);
		close(_resFd[0]);

		args[0] = &_cgiExec[0];
		args[1] = &_cgiPath[0];
		args[2] = NULL;
		execve(_cgiExec.c_str(), args, _env);
	}
	else
	{
		fcntl(_reqFd[1], F_SETFL, O_NONBLOCK);
		fcntl(_resFd[0], F_SETFL, O_NONBLOCK);
		close(_reqFd[0]);
		close(_resFd[1]);
		kqueue.setFdset(_reqFd[1], CGI);
		kqueue.setFdset(_resFd[0], CGI);
		kqueue.addEvent(_reqFd[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, static_cast<void *>(this));
		kqueue.addEvent(_resFd[0], EVFILT_READ, EV_ADD | EV_DISABLE, 0, 0, static_cast<void *>(this));
	}
}
