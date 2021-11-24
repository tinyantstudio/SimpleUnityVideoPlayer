using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;

// Native Player Export
// Referenced DLL must be in PATH or in the PLUGINS folder
// if not we will get DllNotFoundExeption
public class LibVideoPlayerExport
{
    [DllImport("nativevideoplayer", EntryPoint = "enter_test_videoPlayer_static_Lib")]
    public static extern int enter_test_videoPlayer_static_Lib();

    [DllImport("nativevideoplayer")]
    public static extern int player_init();

    [DllImport("nativevideoplayer")]
    public static extern int player_startPlayVideo(string filepath);

    [DllImport("nativevideoplayer")]
    public static extern int player_renderOneFrame();

    [DllImport("nativevideoplayer")]
    public static extern IntPtr player_getOneFrameBuffer();

    // we need to free frame buffer data alloc in native C++
    [DllImport("nativevideoplayer")]
    public static extern void player_getOneFrameBuffer_Done(IntPtr data);

    [DllImport("nativevideoplayer")]
    public static extern int player_shutdown();

    [DllImport("nativevideoplayer")]
    public static extern int player_get_width();

    [DllImport("nativevideoplayer")]
    public static extern int player_get_height();

    [DllImport("nativevideoplayer")]
    public static extern ulong player_get_duration();

    [DllImport("nativevideoplayer")]
    public static extern void player_setconfig([MarshalAs(UnmanagedType.Bool)] bool splityuv);

    [DllImport("nativevideoplayer")]
    public static extern System.IntPtr get_texture_callback_yuv_plane();

    // for debug get YUV buffer
    [DllImport("nativevideoplayer")]
    public static extern IntPtr player_peek_y_buffer();

    [DllImport("nativevideoplayer")]
    public static extern IntPtr player_peek_u_buffer();

    [DllImport("nativevideoplayer")]
    public static extern IntPtr player_peek_v_buffer();

    [DllImport("nativevideoplayer")]
    public static extern IntPtr player_get_pop_y_buffer();

    [DllImport("nativevideoplayer")]
    public static extern IntPtr player_get_pop_u_buffer();

    [DllImport("nativevideoplayer")]
    public static extern IntPtr player_get_pop_v_buffer();
}