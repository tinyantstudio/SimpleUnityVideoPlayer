#include <iostream>
#include "chatEngineExport.h"
#include "chatEngine.h"

void chatEngine::startEngine()
{
	if (!_hasInit)
	{
		_hasInit = true;
	}
}

void chatEngine::shutDownEngine()
{
	if (_hasInit)
	{
	}
}

void chatEngine::createuser(std::string username, std::string pwd)
{
	size_t size = _usernames.size();
	bool find = false;
	for (size_t i = 0; i < size; i++)
	{
		if (_usernames[i] == username)
		{
			find = true;
			break;
		}
	}
	if (!find)
	{
		_usernames.push_back(username);
		_userpwds.push_back(pwd);
	}
}

void chatEngine::login(std::string username, std::string pwd)
{

}

void chatEngine::logout(std::string username)
{
}

void chatEngine::sendmessage(std::string fromusername, std::string message)
{

}