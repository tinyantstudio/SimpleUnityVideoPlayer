#include "chatEngineExport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

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
	newperson->num = 10;
	return newperson;
}

void tool_modify_person(lp_tool_person_info data)
{
	data->infor->messagestr = "from C++ message str";
	// Exception in Unity3D Just fix it
	// data->infor->messagestr = "from C++ message strÕ½¹úÆßÐÛ£¬ÇØÊ¼»Ê!";
	data->infor->namestr = "C++ man";
	data->infor->bvalue = true;

	data->person->firstName = "C";
	data->person->secondName = "++";

	data->vals[0] = 1;
	data->vals[1] = -1;
	data->vals[2] = 1;
}

int tool_get_info(tool_info info)
{
	return info.ivalue;
}