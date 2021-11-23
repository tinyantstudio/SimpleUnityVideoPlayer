#ifndef CHATENGINE_H
#define CHATENGINE_H

#include <string>
#include <vector>

using namespace std;

// 
// Only Test for C# Interop with Native Libraries
//

class chatEngine
{
private:
	bool _hasInit;
	std::vector<string> _usernames;
	std::vector<string> _userpwds;

public:
	void startEngine();
	void shutDownEngine();

	void createuser(std::string username, std::string pwd);

	void login(std::string username, std::string pwd);
	void logout(std::string username);

	void sendmessage(std::string fromusername, std::string message);
};

#endif
