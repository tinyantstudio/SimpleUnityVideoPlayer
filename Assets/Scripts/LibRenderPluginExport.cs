using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;

public class LibRenderPluginExport
{
#if (UNITY_IOS || UNITY_TVOS || UNITY_WEBGL) && !UNITY_EDITOR
    private const string NativePlugInName = "__Internal";
#else
    private const string NativePlugInName = "renderplugin";
#endif

    [DllImport(NativePlugInName)]
    public static extern void SetTimeFromUnity(float t);

    [DllImport(NativePlugInName)]
    public static extern void SetTextureFromUnity(System.IntPtr texture, int w, int h);

    [DllImport(NativePlugInName)]
    public static extern void SetYUVTextureFromUnity(IntPtr ytexture, IntPtr uTexture, IntPtr vTexture, int ywidth,
        int yheight, int uvwidth, int uvheight);

    [DllImport(NativePlugInName)]
    public static extern IntPtr GetRenderEventFunc();
}