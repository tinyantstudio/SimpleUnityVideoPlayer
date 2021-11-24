using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System;

// 
// Video Player Render with Render API BackEnd
//
public class VideoPlayer_RenderAPI : SimpleVideoPlayer
{
    protected override string TAG
    {
        get { return "VideoPlayer_RenderAPI"; }
    }

    enum CustomEventType
    {
        kCustom_Update_YUV = 1 << 10,
        kCustom_VideoPlayerValid = 1 << 11,
        kCustom_VideoPlayerInvalid = 1 << 12
    }

    protected override void Start()
    {
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

        LibRenderPluginExport.SetYUVTextureFromUnity(
            mYTexture.GetNativeTexturePtr(),
            mUTexture.GetNativeTexturePtr(),
            mVTexture.GetNativeTexturePtr(),
            _texYWidth,
            _texYHeight,
            _texUVWidth,
            _texUVHeight);

        GL.IssuePluginEvent(LibRenderPluginExport.GetRenderEventFunc(), (int)CustomEventType.kCustom_VideoPlayerValid);
        // notify render API video player is ready
        while (_curPlayTime < _playTotalTime)
        {
            int ret = LibVideoPlayerExport.player_renderOneFrame();
            if (ret == 0)
            {
                GL.IssuePluginEvent(LibRenderPluginExport.GetRenderEventFunc(),
                    (int)CustomEventType.kCustom_Update_YUV);
                // Debug.Log("render one frame...");
            }

            RenderVideoFrameBlitYUV(scale);
            yield return _coroutine_time_interval;
            _curPlayTime += _timeinterval;
        }
    }

    protected override void OnDestroy()
    {
        GL.IssuePluginEvent(LibRenderPluginExport.GetRenderEventFunc(),
            (int)CustomEventType.kCustom_VideoPlayerInvalid);
        base.OnDestroy();
    }
}