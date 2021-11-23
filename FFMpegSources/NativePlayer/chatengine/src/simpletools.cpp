#include "chatEngineExport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

enum cal_type
{
	cal_type_add = 0,
	cal_type_sub,
	cal_type_mul,
	cal_type_div
};

// simple tools 
void tool_c_strncpy(char* dest, const char* src, size_t n)
{
	strncpy(dest, src, n);
}

void tool_do_calculate(int a, int b, int caltype, fn_tool_callback_int callback)
{
	int ret = 0;
	std::string message = "success!";
	if (caltype == static_cast<int>(cal_type_add))
	{
		ret = a + b;
	}
	else if (caltype == static_cast<int>(cal_type_sub))
	{
		ret = a - b;
	}
	else if (caltype == static_cast<int>(cal_type_mul))
	{
		ret = a * b;
	}
	else if (caltype == static_cast<int>(cal_type_div))
	{
		ret = a / b;
	}
	else
	{
		message = "not support cal type: " + caltype;
	}
	if (callback != NULL)
	{
		callback(ret, message.c_str());
	}
}

void* tool_get_one_person(const char* firstName, const char* secondName, bool man, int age)
{
	lp_tool_person newperson = new tool_person();
	newperson->age = age;
	newperson->firstName = (char*)firstName;
	newperson->secondName = (char*)secondName;
	newperson->num = 100000;
	return newperson;
}

void tool_modify_person(lp_tool_person_info data)
{
	data->infor->messagestr = "Message From C++ : Say Hello To C#";
	// Exception in Unity3D Just fix it
	data->infor->messagestr = "from C++ message str...";
	data->infor->namestr = "C++ man";
	data->infor->bvalue = true;

	data->person->firstName = "Jack-Jack";
	data->person->secondName = "MA-WA";

	data->vals[0] = 1;
	data->vals[1] = -1;
	data->vals[2] = 1;
}

int tool_get_info(tool_info info)
{
	return info.ivalue;
}

void* tool_get_string(const char* inputstring)
{
	char* str = "Hello world Native C++!";
	return (void*)str;
	// return NULL;
}

int tool_get_pow(int a, int b)
{
	return pow(a, b);
}

static std::vector<mesh_vertex> mesh_vertex_vector;

void tool_pass_array(void *meshvertex, int vertexcount, float* posBuff, float* normalBuff)
{
	mesh_vertex_vector.resize(vertexcount);
	for (int i = 0; i < vertexcount; i++)
	{
		mesh_vertex& item = mesh_vertex_vector[i];
		item.pos[0] = posBuff[0];
		item.pos[1] = posBuff[1];
		item.pos[2] = posBuff[2];

		item.normal[0] = normalBuff[0];
		item.normal[1] = normalBuff[1];
		item.normal[2] = normalBuff[2];

		posBuff += 3;
		normalBuff += 3;
	}
}

// https://limbioliong.wordpress.com/2011/06/16/returning-strings-from-a-c-api/
// C++ can't just malloc to return char* 
// should alloc unmanaged string by 
// 1. CoTaskMemAlloc()/Marshal.FreeCoTaskMem().
// 2. SysAllocString/Marshal.FreeBSTR().
// we need just delegate or reference to pass string char* from C++ to C#
// https://stackoverflow.com/questions/393509/usage-of-cotaskmemalloc
// Use CoTaskMemAlloc when returning a char* from a native C++ library to.NET as a string.
// Since .NET uses CoTaskMemFree, you have to allocate the string like this, 
// you can't allocate it on the stack or the heap using malloc / new

char* tool_get_vertex_pos()
{
	std::string message;
	std::stringstream ss;
	size_t size = mesh_vertex_vector.size();

	int i = 0;
	for (auto& item : mesh_vertex_vector)
	{
		item.pos[0] = 1 * i;
		item.pos[1] = 10 * i;
		item.pos[2] = 100 * i;

		item.normal[0] = 1 * i;
		item.normal[1] = 10 * i;
		item.normal[2] = 100 * i;

		ss << i << "|";
		ss << item.pos[0] << "," << item.pos[1] << "," << item.pos[2] << "|";
		ss << item.normal[0] << "," << item.normal[1] << "," << item.normal[2] << "|";
		ss << ";";
		i++;
	}
	ss >> message;
	return const_cast<char*>(message.c_str());
}

void tool_get_vertex_pos_callback(fn_tool_callback_int callback)
{
	std::string message;
	std::stringstream ss;
	size_t size = mesh_vertex_vector.size();

	int i = 0;
	for (auto& item : mesh_vertex_vector)
	{
		item.pos[0] = 1 * i;
		item.pos[1] = 10 * i;
		item.pos[2] = 100 * i;

		item.normal[0] = 1 * i;
		item.normal[1] = 10 * i;
		item.normal[2] = 100 * i;

		ss << i << "|";
		ss << item.pos[0] << "," << item.pos[1] << "," << item.pos[2] << "|";
		ss << item.normal[0] << "," << item.normal[1] << "," << item.normal[2] << "|";
		ss << ";";
		i++;
	}
	ss >> message;
	callback((int)size, message.c_str());
}



