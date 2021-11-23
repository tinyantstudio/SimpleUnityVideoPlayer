#ifndef CHATENGINE_EXPORT_H
#define CHATENGINE_EXPORT_H

// 
// Interop with Native Libraries
// demo for https://www.mono-project.com/docs/advanced/pinvoke/
// 

// https://docs.microsoft.com/en-us/cpp/build/exporting-c-functions-for-use-in-c-or-cpp-language-executables?view=msvc-170

#ifdef CHATENGINELIB_EXPORTS
#define CHATENGINELIB_API __declspec(dllexport)
#else
#define CHATENGINELIB_API __declspec(dllimport)
#endif



// just for Native Interop with C#

extern "C"
{
	// C++ function pointer need C# delegate
	typedef void(*fn_login_callback)(int code, char* messsage);
	typedef void(*fn_logout_callback)(int code, char* message);
	typedef void(*fn_sendmessage_result_callback)(int code);
	typedef void(*fn_receivemessage_callback)(int code, char* message);

	int CHATENGINELIB_API chat_engine_boot();
	int CHATENGINELIB_API chat_engine_shutdown();
	int CHATENGINELIB_API chat_login(char* username, char* pwd, fn_login_callback callback);
	int CHATENGINELIB_API chat_logout(char* username);
	int CHATENGINELIB_API chat_sendMessage(char* username, char* message, fn_sendmessage_result_callback callback);
	void CHATENGINELIB_API chat_register_broadcast_msg(fn_receivemessage_callback callback);
}

// struct
typedef struct tool_struct_infor
{
	bool bvalue;
	int ivalue;
	int x;
	int y;
	char* messagestr;
	char* namestr;
}tool_info, *lp_tool_info;


typedef struct tool_struct_person
{
	char* firstName;
	char* secondName;
	int age;
	int num;
}tool_person, *lp_tool_person;


typedef struct tool_struct_person_info
{
	lp_tool_person person;
	lp_tool_info infor;
	int vals[3];
	char* infortitle;
}tool_person_info, *lp_tool_person_info;


typedef struct tool_mesh_vertex
{
	float pos[3];
	float normal[3];
}mesh_vertex;

extern "C" {
	// simple tools
	CHATENGINELIB_API void tool_c_strncpy(char* dest, const char* src, size_t n);

	typedef void(*fn_tool_callback_int)(int value, const char* message);
	void CHATENGINELIB_API tool_do_calculate(int a, int b, int caltype, fn_tool_callback_int callback);

	CHATENGINELIB_API void* tool_get_one_person(const char* firstName, const char* secondName, bool man, int age);
	CHATENGINELIB_API void tool_modify_person(lp_tool_person_info data);
	CHATENGINELIB_API int tool_get_info(tool_info info);
	CHATENGINELIB_API void* tool_get_string(const char* inputstring);
	CHATENGINELIB_API int tool_get_pow(int a, int b);
	CHATENGINELIB_API void tool_pass_array(void *meshvertex, int vertexcount, float* pos, float* normal);
	CHATENGINELIB_API char* tool_get_vertex_pos();
	CHATENGINELIB_API void tool_get_vertex_pos_callback(fn_tool_callback_int callback);
}

#endif // !CHATENGINE_H
