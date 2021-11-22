using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine;
using UnityEngine.Analytics;
using UnityEngine.Rendering;
using UnityEngine.UI;

// Referenced DLL must be in PATH or in the PLUGINS folder
// if not we will get DllNotFoundExeption
public class VideoPlayer_UnityCommandBuf : SimpleVideoPlayer
{
    CommandBuffer _command;
    public RawImage _yDebugImage;
    public RawImage _uDebugImage;
    public RawImage _vDebugImage;

    protected override string TAG
    {
        get { return "SimpleVideoPlayer-CommandBuffer"; }
    }

    protected override void Start()
    {
        _command = new CommandBuffer();
        base.Start();
    }

    protected override IEnumerator _StartPlayVideo()
    {
        yield return new WaitForSeconds(0.5f);
        SetUp();
        bool suc = InitPlayer(RENDER_TYPE.CommandBuffer);
        if (!suc)
        {
            SimpleDebuger.LogError(TAG, "Init Native Player Fatal Error...");
            yield break;
        }

        CreateTexture(TEX_YUV_TYPE.Y, _texYWidth, _texYHeight);
        CreateTexture(TEX_YUV_TYPE.U, _texUVWidth, _texUVHeight);
        CreateTexture(TEX_YUV_TYPE.V, _texUVWidth, _texUVHeight);

        _yDebugImage.texture = mYTexture;
        _uDebugImage.texture = mUTexture;
        _vDebugImage.texture = mVTexture;

        if (mYTexture == null || mUTexture == null || mVTexture == null)
        {
            yield break;
        }

        IntPtr callBack = LibVideoPlayerExport.get_texture_callback_yuv_plane();
        if (callBack == IntPtr.Zero)
        {
            yield break;
        }

        int ybufsize = _texYHeight * _texYWidth;
        int ubufsize = _texYHeight * _texYWidth / 4;
        int vbufsize = _texYHeight * _texYWidth / 4;

        byte[] ybuff = new byte[ybufsize];
        byte[] ubuff = new byte[ubufsize];
        byte[] vbuff = new byte[vbufsize];

        while (_curPlayTime < _playTotalTime)
        {
            // _command.IssuePluginCustomTextureUpdateV2(callBack,
            //     mYTexture, (uint)0);
            // _command.IssuePluginCustomTextureUpdateV2(callBack,
            //     mUTexture, (uint)1);
            // _command.IssuePluginCustomTextureUpdateV2(callBack,
            //     mVTexture, (uint)2);
            // int ret = LibVideoPlayerExport.player_renderOneFrame();
            // if (ret == 0)
            // {
            //     IntPtr yptr = LibVideoPlayerExport.player_get_y_buffer();
            //     IntPtr uptr = LibVideoPlayerExport.player_get_u_buffer();
            //     IntPtr vptr = LibVideoPlayerExport.player_get_v_buffer();
            //
            //     if (yptr != IntPtr.Zero && uptr != IntPtr.Zero && vptr != IntPtr.Zero)
            //     {
            //         Marshal.Copy(yptr, ybuff, 0, ybufsize);
            //         Marshal.Copy(uptr, ubuff, 0, ubufsize);
            //         Marshal.Copy(vptr, vbuff, 0, vbufsize);
            //
            //         mYTexture.SetPixelData(ybuff, 0, 0);
            //         mUTexture.SetPixelData(ubuff, 0, 0);
            //         mVTexture.SetPixelData(vbuff, 0, 0);
            //
            //         mYTexture.Apply(false);
            //         mUTexture.Apply(false);
            //         mVTexture.Apply(false);
            //
            //         RenderVideoFrameBlit();
            //         Debug.Log("render one frame...");
            //     }
            // }

            // IssuePluginCustomTextureUpdateV2 command can't be Async just be sync!!!
            // so it will block GPU if command time comsuming!!!
            // so don't push so hard like [4K 60fps]!!!
            // this.ValidateAgainstExecutionFlags(CommandBufferExecutionFlags.None, CommandBufferExecutionFlags.AsyncCompute);

            // Graphics.ExecuteCommandBufferAsync(_command, ComputeQueueType.Background);
            // Graphics.ExecuteCommandBuffer(_command);
            // _command.Clear();
            // Debug.Log("render frame...");
            // int ret = LibVideoPlayerExport.player_renderOneFrame();
            // Debug.Log("render frame...: " + ret);
            yield return new WaitForSeconds(_timeinterval);
            _curPlayTime += _timeinterval;
        }
    }
}