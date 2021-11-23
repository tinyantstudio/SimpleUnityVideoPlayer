using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.UI;

public class VideoPlayer_UnityCommandBuf : SimpleVideoPlayer
{
    CommandBuffer _command;
    protected override string TAG
    {
        get { return "SimpleVideoPlayer-CommandBuffer"; }
    }

    protected override void Start()
    {
        _command = new CommandBuffer();
        // see CPU usage Profiler Window timeline we will find renderthread mark [NativePlayer_Update_Texture]
        _command.name = "NativePlayer_Update_Texture";
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

        // cache coroutine reduce GC
        _coroutine_time_interval = new WaitForSeconds(_timeinterval);

        CreateTexture(TEX_YUV_TYPE.Y, _texYWidth, _texYHeight);
        CreateTexture(TEX_YUV_TYPE.U, _texUVWidth, _texUVHeight);
        CreateTexture(TEX_YUV_TYPE.V, _texUVWidth, _texUVHeight);

        _yDebugImage.texture = mYTexture;
        _uDebugImage.texture = mUTexture;
        _vDebugImage.texture = mVTexture;

        float scale = _texYWidth > 1920 ? 0.75f : 1.0f;

        if (mYTexture == null || mUTexture == null || mVTexture == null)
        {
            yield break;
        }

        IntPtr callBack = LibVideoPlayerExport.get_texture_callback_yuv_plane();
        if (callBack == IntPtr.Zero)
        {
            yield break;
        }

        while (_curPlayTime < _playTotalTime)
        {
            int ret = LibVideoPlayerExport.player_renderOneFrame();
            if (ret == 0)
            {
                // Debug.Log("render one frame...");
                _command.IssuePluginCustomTextureUpdateV2(callBack,
                    mYTexture, (uint)0);
                _command.IssuePluginCustomTextureUpdateV2(callBack,
                    mUTexture, (uint)1);
                _command.IssuePluginCustomTextureUpdateV2(callBack,
                    mVTexture, (uint)2);
                Graphics.ExecuteCommandBuffer(_command);
                _command.Clear();

                // IssuePluginCustomTextureUpdateV2 command can't be Async just be sync!!!
                // so it will block GPU if command time comsuming!!!
                // so don't push so hard like [4K 60fps]!!!
                // this.ValidateAgainstExecutionFlags(CommandBufferExecutionFlags.None, CommandBufferExecutionFlags.AsyncCompute);
                // Graphics.ExecuteCommandBufferAsync(_command, ComputeQueueType.Background);
                // Graphics.ExecuteCommandBuffer(_command);
                // _command.Clear();
                RenderVideoFrameBlitYUV(scale);
            }

            yield return _coroutine_time_interval;
            _curPlayTime += _timeinterval;
        }
    }
}