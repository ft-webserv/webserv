#include "Response.hpp"

Response::Response()
{
}

Response::~Response()
{
}

void Response::handleGet(ServerInfo *servInfo, Request *req)
{
	std::string root;
	std::vector<LocationInfo *> tmp = servInfo->getLocationInfo();
	LocationInfo *loc = NULL;

	root = servInfo->getServerInfo().find("root")->second;
	if (tmp.size() == 0)
	{
		req->
	}
	else
	{
		for (int i = 0; i < tmp.size(); i++) // LocationInfo 의 _path와 요청의 URL이 같은 location 블록 찾는 부분
		{
			if (tmp[i]->getPath() == req->getParsedRequest()._location)
				loc = tmp[i];
		}
		if (loc == NULL && tmp[0]) // 만약 못찾았고, server block에 location block이 하나라도 있으면 첫번째 location블록으로 설정
			loc = tmp[0];
	}
}

void Response::handlePost()
{
}

void Response::handleDelete()
{
}
