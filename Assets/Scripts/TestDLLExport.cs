using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

public class LibVideoPlayerExport
{
    [DllImport("videoPlayerLib", EntryPoint = "make_sub_please")]
    public static extern int make_sub_please(int a, int b);

    [DllImport("videoPlayerLib", EntryPoint = "enter_test_videoPlayer_static_Lib")]
    public static extern int enter_test_videoPlayer_static_Lib();

    [DllImport("CalltoCall")]
    public static extern int add_value(int a, int b);
}

// Referenced DLL must be in PATH or in the PLUGINS folder
// if not we will get DllNotFoundExeption
public class TestDLLExport : MonoBehaviour
{
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.S))
        {
            int ret = LibVideoPlayerExport.add_value(100, 200);
            Debug.Log("add_value ret: " + ret);
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