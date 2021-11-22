using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;

public class LibVideoPlayerExport
{
    [DllImport("videoPlayerLib", EntryPoint = "enter_test_videoPlayer_static_Lib")]
    public static extern int enter_test_videoPlayer_static_Lib();

    [DllImport("videoPlayerLib")]
    public static extern int player_init();

    [DllImport("videoPlayerLib")]
    public static extern int player_startPlayVideo(string filepath);

    [DllImport("videoPlayerLib")]
    public static extern int player_renderOneFrame();

    [DllImport("videoPlayerLib")]
    public static extern IntPtr player_getOneFrameBuffer();

    // we need to free frame buffer data alloc in native C++
    [DllImport("videoPlayerLib")]
    public static extern void player_getOneFrameBuffer_Done(IntPtr data);

    [DllImport("videoPlayerLib")]
    public static extern int player_shutdown();

    [DllImport("videoPlayerLib")]
    public static extern int player_get_width();

    [DllImport("videoPlayerLib")]
    public static extern int player_get_height();

    [DllImport("videoPlayerLib")]
    public static extern ulong player_get_duration();

    [DllImport("videoPlayerLib")]
    public static extern void player_setconfig([MarshalAs(UnmanagedType.Bool)] bool splityuv);

    [DllImport("videoPlayerLib")]
    public static extern System.IntPtr get_texture_callback_yuv_plane();
}