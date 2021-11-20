using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using TMPro.EditorUtilities;
using UnityEngine;

// 
// Lib Video Player Export Interface
//
public class LibVideoPlayerExport
{
    [DllImport("videoPlayerLib", EntryPoint = "make_sub_please")]
    public static extern int make_sub_please(int a, int b);

    [DllImport("videoPlayerLib", EntryPoint = "enter_test_videoPlayer_static_Lib")]
    public static extern int enter_test_videoPlayer_static_Lib();
}

// 调用方将清理堆栈 cdecl
// 被调用方将清理堆栈 StdCall
[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
public delegate void tool_callback_int(int value, string message);

public class LibChatExport
{
    [DllImport("chatEngineLib")]
    public static extern void tool_c_strncpy(StringBuilder dest, string src, uint n);

    [DllImport("chatEngineLib")]
    public static extern void tool_do_calculate(int a, int b, int caltype, tool_callback_int callback);

    [DllImport("chatEngineLib")]
    public static extern IntPtr tool_get_one_person(string firstName, string secondName, bool man, int age);

    [DllImport("chatEngineLib")]
    public static extern void tool_modify_person(ref MyPerson_Info infor);

    [DllImport("charEngineLib")]
    public static extern int tool_get_info(tools_struct_person person);
}

//
// Lib Chat Export Interface
//

[StructLayout(LayoutKind.Sequential)]
public struct tools_struct_infor
{
    // just use one byte
    [MarshalAs(UnmanagedType.I1)] public bool bvalue;
    public int ivalue;
    public int x;
    public int y;
    public string messagestr;
    public string namestr;
}

[StructLayout(LayoutKind.Sequential)]
public struct tools_struct_person
{
    public string firstName;
    public string secondName;
    public int age;
    public int num;
}

[StructLayout(LayoutKind.Sequential)]
public struct MyPerson_Info
{
    public IntPtr person;
    public IntPtr infor;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
    public int[] vals;

    public string infortitle;
}

// Referenced DLL must be in PATH or in the PLUGINS folder
// if not we will get DllNotFoundExeption
public class TestDLLExport : MonoBehaviour
{
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.S))
        {
            tools_struct_infor infor = new tools_struct_infor();
            infor.bvalue = false;
            infor.ivalue = 1000000;
            infor.x = 1111;
            infor.y = 2222;
            infor.messagestr = "Jack";
            infor.namestr = "Ma";
            infor.x = 10;
            infor.y = 100;

            tools_struct_person person = new tools_struct_person();
            person.age = 10;
            person.firstName = "Fun";
            person.secondName = "Shit";
            person.num = 10;

            int size = Marshal.SizeOf(infor);
            Debug.Log("size: " + size);
            IntPtr ptrInforBuffer = Marshal.AllocCoTaskMem(Marshal.SizeOf(infor));
            Marshal.StructureToPtr(infor, ptrInforBuffer, false);

            IntPtr ptrPersonBuffer = Marshal.AllocCoTaskMem(Marshal.SizeOf(person));
            Marshal.StructureToPtr(person, ptrPersonBuffer, false);

            MyPerson_Info personInfo = new MyPerson_Info();
            personInfo.infor = ptrInforBuffer;
            personInfo.person = ptrPersonBuffer;
            personInfo.infortitle = "My MAN";

            Debug.Log(
                $"before--->name: {person.firstName}, secondname: {person.secondName}, age: {person.age}");
            Debug.Log(
                $"before--->message: {infor.messagestr}, bvalue: {infor.bvalue}, intvalue: {infor.ivalue}, x: {infor.x}, y: {infor.y}");
            // Debug.Log($"before---> array: ");
            // foreach (var value in personInfo.vals)
            // {
            //     Debug.Log(value);
            // }

            LibChatExport.tool_modify_person(ref personInfo);

            tools_struct_person personaftermodify =
                (tools_struct_person) Marshal.PtrToStructure(personInfo.person, typeof(tools_struct_person));
            infor = (tools_struct_infor) Marshal.PtrToStructure(personInfo.infor, typeof(tools_struct_infor));
            Debug.Log(
                $"after--->name: {personaftermodify.firstName}, secondname: {personaftermodify.secondName}, age: {personaftermodify.age}");
            Debug.Log(
                $"after--->message: {infor.messagestr}, bvalue: {infor.bvalue}, intvalue: {infor.ivalue}, x: {infor.x}, y: {infor.y}");
            Debug.Log($"after---> array: ");
            foreach (var value in personInfo.vals)
            {
                Debug.Log(value);
            }

            Marshal.FreeCoTaskMem(ptrInforBuffer);
            Marshal.FreeCoTaskMem(ptrPersonBuffer);
        }

        if (Input.GetKeyDown(KeyCode.D))
        {
            int ret = LibVideoPlayerExport.make_sub_please(500, 200);
            Debug.Log("make_sub_please ret: " + ret);
        }

        if (Input.GetKeyDown(KeyCode.J))
        {
            int ret = LibVideoPlayerExport.enter_test_videoPlayer_static_Lib();
            Debug.Log("enter_test_videoPlayer_static_Lib ret: " + ret);
        }
    }
}