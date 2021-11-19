#include "chatEngineExport.h"


int chat_engine_boot()
{
	return 0;
}

int chat_engine_shutdown()
{
	return 0;
}

int chat_login(char* username, char* pwd, fn_login_callback callback)
{
	return 0;
}

int chat_logout(char* username)
{
	return 0;
}

int chat_sendMessage(char* username, char* message, fn_sendmessage_result_callback callback)
{
	return 0;
}

void chat_register_broadcast_msg(fn_receivemessage_callback callback)
{
}