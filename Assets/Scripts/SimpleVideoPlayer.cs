using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Runtime.InteropServices;
using System;
using System.Diagnostics;
using UnityEngine.Analytics;
using Debug = UnityEngine.Debug;
using Object = UnityEngine.Object;
using UnityEngine.Profiling;

public class SimpleVideoPlayer : MonoBehaviour
{
    public string mediaPath = string.Empty;
    protected float _timeinterval = 1 / 25f;
    public float _playTotalTime = 10;
    public bool _customTotalTime = false;
    public float _curPlayTime = 0;
    public int _playFPS = 30;
    public int _CurFrameCount = 0;
    public RawImage _targetRawImageToShowFrame;
    protected Matrix4x4 _YUVToRGBMatrix;
    public Material mBlitYUVToRGBMat;
    protected RenderTexture _renderTexture;

    // YUV texture
    // the right and effective way is just create texture one time and udpate the texture data
    protected Texture2D mYTexture;
    protected Texture2D mUTexture;
    protected Texture2D mVTexture;

    protected int _texYWidth;
    protected int _texYHeight;
    protected int _texUVWidth;
    protected int _texUVHeight;

    public enum TEX_YUV_TYPE
    {
        Y = 0,
        U,
        V
    }

    public enum RENDER_TYPE
    {
        CPU_Normal,
        CommandBuffer,
    }

    protected virtual string TAG
    {
        get { return "SimpleVideoPlayer-CPU"; }
    }

    protected virtual void Start()
    {
        StartCoroutine(_StartPlayVideo());
    }

    protected void OnDestroy()
    {
        SimpleDebuger.LogInfo(TAG, "OnDestroy() shutdown native player...");
        LibVideoPlayerExport.player_shutdown();
    }

    protected void CreateTexture(TEX_YUV_TYPE type, int width, int height)
    {
        switch (type)
        {
            case TEX_YUV_TYPE.Y:
                if (mYTexture == null || (mYTexture.width * mYTexture.height) != (width * height))
                {
                    if (mYTexture != null)
                        Object.DestroyImmediate(mYTexture);
                    mYTexture = new Texture2D(width, height, TextureFormat.R8, false);
                }

                break;
            case TEX_YUV_TYPE.U:
            case TEX_YUV_TYPE.V:
                Texture2D target = type == TEX_YUV_TYPE.U ? mUTexture : mVTexture;
                if (target == null || (target.width * target.height) != (width * height))
                {
                    if (target != null)
                        Object.DestroyImmediate(target);
                    target = new Texture2D(width, height, TextureFormat.R8, false);
                    if (type == TEX_YUV_TYPE.U) mUTexture = target;
                    else mVTexture = target;
                }

                break;
            default:
                break;
        }
    }

    protected void SetUpMatrix()
    {
        // convert YUV to RGB matrix
        SimpleDebuger.LogInfo(TAG, "SetUpMatrix()");
        _YUVToRGBMatrix.SetRow(0, new Vector4(1, 0, 1.4022f, 0));
        _YUVToRGBMatrix.SetRow(1, new Vector4(1, -0.3456f, -0.7145f, 0));
        _YUVToRGBMatrix.SetRow(2, new Vector4(1, 1.771f, 0, 0));
        _YUVToRGBMatrix.SetRow(3, new Vector4(0, 0, 0, 1));
    }

    protected void CreateRenderTexture(int width, int height)
    {
        SimpleDebuger.LogInfo(TAG, "CreateRenderTexture() width: " + width + ",height: " + height);
        _renderTexture = new RenderTexture(width, height, 0, RenderTextureFormat.ARGB32);
    }

    protected virtual void SetUp()
    {
        SimpleDebuger.LogInfo(TAG, "_StartPlayVideo() start");
        SetUpMatrix();

        // config
        _timeinterval = 1.0f / _playFPS;
        SimpleDebuger.LogInfo(TAG, "video play FPS: " + _playFPS + ",interval: " + _timeinterval);
        _curPlayTime = 0f;
        _CurFrameCount = 0;
    }

    protected virtual bool InitPlayer(RENDER_TYPE renderType)
    {
        int ret = LibVideoPlayerExport.player_init();
        if (ret != 0)
            return false;
        ret = LibVideoPlayerExport.player_startPlayVideo(mediaPath);
        if (ret != 0)
            return false;
        ulong duration = LibVideoPlayerExport.player_get_duration();
        if (!_customTotalTime)
            _playTotalTime = duration * 1f;
        Debug.Log("duration: " + duration);

        int videoWidth = LibVideoPlayerExport.player_get_width();
        int videoHeight = LibVideoPlayerExport.player_get_height();

        _texYWidth = videoWidth;
        _texYHeight = videoHeight;

        _texUVWidth = videoWidth / 2;
        _texUVHeight = videoHeight / 2;

        CreateRenderTexture(videoWidth, videoHeight);
        LibVideoPlayerExport.player_setconfig(renderType == RENDER_TYPE.CommandBuffer);
        return true;
    }

    protected virtual IEnumerator _StartPlayVideo()
    {
        yield return new WaitForSeconds(0.5f);
        SetUp();
        bool suc = InitPlayer(RENDER_TYPE.CPU_Normal);
        if (!suc)
        {
            SimpleDebuger.LogError(TAG, "Init Native Player Fatal Error...");
            yield break;
        }

        int totalBuffSize = (int)(_texYWidth * _texYHeight * 1.5f);
        SimpleDebuger.LogInfo(TAG, "video width: " + _texYWidth + ",height: " + _texYHeight);
        int ybufsize = _texYHeight * _texYWidth;
        int ubufsize = _texYHeight * _texYWidth / 4;
        int vbufsize = _texYHeight * _texYWidth / 4;
        // create YUV texture buffer
        byte[] targetbuf = new byte[totalBuffSize];
        byte[] ybuff = new byte[ybufsize];
        byte[] ubuff = new byte[ubufsize];
        byte[] vbuff = new byte[vbufsize];

        Stopwatch sw = new Stopwatch();
        while (_curPlayTime < _playTotalTime)
        {
            sw.Restart();
            int ret = LibVideoPlayerExport.player_renderOneFrame();
            sw.Stop();
            Debug.Log($"render one frame take: {sw.ElapsedMilliseconds}ms");
            if (ret != 0)
            {
                SimpleDebuger.LogInfo(TAG, "skip frame: " + _CurFrameCount);
            }
            else
            {
                IntPtr buffptr = LibVideoPlayerExport.player_getOneFrameBuffer();
                if (buffptr == IntPtr.Zero)
                {
                    SimpleDebuger.LogInfo(TAG,
                        "render frame: " + _CurFrameCount + ",success but buffer data is null, so just skip");
                }
                else
                {
                    // just copy frame native array
                    Marshal.Copy(buffptr, targetbuf, 0, totalBuffSize);
                    Buffer.BlockCopy(targetbuf, 0, ybuff, 0, ybufsize);
                    Buffer.BlockCopy(targetbuf, ybufsize, ubuff, 0, ubufsize);
                    Buffer.BlockCopy(targetbuf, ybufsize + ubufsize, vbuff, 0, vbufsize);

                    //
                    // 1 way---->. every frame to create new texture2d with input buffer by LoadRawTextureData
                    //
                    // if (mYTexture != null)
                    //     Object.DestroyImmediate(mYTexture);
                    // if (mUTexture != null)
                    //     Object.DestroyImmediate(mUTexture);
                    // if (mVTexture != null)
                    //     Object.DestroyImmediate(mVTexture);

                    // mYTexture = new Texture2D(videoWidth, videoHeight, TextureFormat.R8, false);
                    // mUTexture = new Texture2D(videoWidth / 2, videoHeight / 2, TextureFormat.R8, false);
                    // mVTexture = new Texture2D(videoWidth / 2, videoHeight / 2, TextureFormat.R8, false);
                    //
                    // mYTexture.LoadRawTextureData(ybuff);
                    // mUTexture.LoadRawTextureData(ubuff);
                    // mVTexture.LoadRawTextureData(vbuff);
                    //

                    //
                    // 2 way. everty frame not to create new texture2d,just using SetPixelData to update texture2d's data
                    //
                    CreateTexture(TEX_YUV_TYPE.Y, _texYWidth, _texYHeight);
                    CreateTexture(TEX_YUV_TYPE.U, _texUVWidth, _texUVHeight);
                    CreateTexture(TEX_YUV_TYPE.V, _texUVWidth, _texUVHeight);

                    if (mUTexture == null || mYTexture == null || mVTexture == null)
                    {
                        SimpleDebuger.LogInfo(TAG, "YUV texture is not valid...");
                        continue;
                    }

                    mYTexture.SetPixelData(ybuff, 0, 0);
                    mUTexture.SetPixelData(ubuff, 0, 0);
                    mVTexture.SetPixelData(vbuff, 0, 0);

                    // Call Apply after setting image data to actually upload it to the GPU.
                    // Apply texture to GPU
                    // this will be time-consuming
                    mYTexture.Apply(false);
                    mUTexture.Apply(false);
                    mVTexture.Apply(false);

                    RenderVideoFrameBlit();

                    LibVideoPlayerExport.player_getOneFrameBuffer_Done(buffptr);
                }
            }

            _CurFrameCount++;
            yield return new WaitForSeconds(_timeinterval);
            _curPlayTime += _timeinterval;
        }
    }

    protected virtual void RenderVideoFrameBlit()
    {
        mBlitYUVToRGBMat.SetTexture("_UTex", mUTexture);
        mBlitYUVToRGBMat.SetTexture("_VTex", mVTexture);
        mBlitYUVToRGBMat.SetMatrix("_YUVMat", _YUVToRGBMatrix);

        RenderTexture pre = RenderTexture.active;
        RenderTexture.active = _renderTexture;
        Graphics.Blit(mYTexture, _renderTexture, mBlitYUVToRGBMat);
        _targetRawImageToShowFrame.texture = _renderTexture;
        _targetRawImageToShowFrame.SetNativeSize();
        RenderTexture.active = pre;
    }
}