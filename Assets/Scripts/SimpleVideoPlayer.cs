using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Runtime.InteropServices;
using System;
using Object = UnityEngine.Object;

public class SimpleVideoPlayer : MonoBehaviour
{
    public string mediaPath = string.Empty;
    private float _timeinterval = 1 / 25f;
    public float _playTotalTime = 10;
    public bool _customTotalTime = false;
    public float _curPlayTime = 0;
    public int _playFPS = 30;
    public int _CurFrameCount = 0;
    public RawImage _targetRawImageToShowFrame;
    private Matrix4x4 _YUVToRGBMatrix;
    public Material mBlitYUVToRGBMat;
    private RenderTexture _renderTexture;


    // YUV texture
    // the right and effective way is just create texture one time and udpate the texture data
    private Texture2D mYTexture;
    private Texture2D mUTexture;
    private Texture2D mVTexture;

    private string TAG = "SimpleVideoPlayer";

    void Start()
    {
        StartCoroutine(_StartPlayVideo());
    }

    void SetUpMatrix()
    {
        SimpleDebuger.LogInfo(TAG, "SetUpMatrix()");
        _YUVToRGBMatrix.SetRow(0, new Vector4(1, 0, 1.4022f, 0));
        _YUVToRGBMatrix.SetRow(1, new Vector4(1, -0.3456f, -0.7145f, 0));
        _YUVToRGBMatrix.SetRow(2, new Vector4(1, 1.771f, 0, 0));
        _YUVToRGBMatrix.SetRow(3, new Vector4(0, 0, 0, 1));
    }

    void CreateRenderTexture(int width, int height)
    {
        SimpleDebuger.LogInfo(TAG, "CreateRenderTexture() width: " + width + ",height: " + height);
        _renderTexture = new RenderTexture(width, height, 0, RenderTextureFormat.ARGB32);
    }

    IEnumerator _StartPlayVideo()
    {
        Debug.LogAssertion(mBlitYUVToRGBMat != null);
        Debug.LogAssertion(_targetRawImageToShowFrame != null);

        SimpleDebuger.LogInfo(TAG, "_StartPlayVideo() start");
        SetUpMatrix();

        // config
        _timeinterval = 1.0f / _playFPS;
        SimpleDebuger.LogInfo(TAG, "video play FPS: " + _playFPS + ",interval: " + _timeinterval);
        _curPlayTime = 0f;
        _CurFrameCount = 0;

        yield return new WaitForSeconds(0.5f);
        int ret = LibVideoPlayerExport.player_init();
        Debug.LogAssertion(ret == 0);
        ret = LibVideoPlayerExport.player_startPlayVideo(mediaPath);
        Debug.LogAssertion(ret == 0);

        ulong duration = LibVideoPlayerExport.player_get_duration();
        if (!_customTotalTime)
            _playTotalTime = duration * 1f;
        Debug.Log("duration: " + duration);

        int videoWidth = LibVideoPlayerExport.player_get_width();
        int videoHeight = LibVideoPlayerExport.player_get_height();
        CreateRenderTexture(videoWidth, videoHeight);

        int totalBuffSize = (int) (videoWidth * videoHeight * 1.5f);

        SimpleDebuger.LogInfo(TAG, "video width: " + videoWidth + ",height: " + videoHeight);

        while (_curPlayTime < _playTotalTime)
        {
            ret = LibVideoPlayerExport.player_renderOneFrame();
            if (ret != 0)
            {
                SimpleDebuger.LogInfo(TAG, "skil frame: " + _CurFrameCount);
            }
            else
            {
                // SimpleDebuger.LogInfo(TAG, "render frame: " + testframecount + ",success");
                IntPtr buffptr = LibVideoPlayerExport.player_getOneFrameBuffer();
                if (buffptr == IntPtr.Zero)
                {
                    SimpleDebuger.LogInfo(TAG,
                        "render frame: " + _CurFrameCount + ",success but buffer data is null, so just skip");
                }
                else
                {
                    // just copy from intptr buffer data no need to create a new byte array
                    byte[] targetbuf = new byte[totalBuffSize];
                    Marshal.Copy(buffptr, targetbuf, 0, totalBuffSize);

                    int ybufsize = videoHeight * videoWidth;
                    int ubufsize = videoHeight * videoWidth / 4;
                    int vbufsize = videoHeight * videoWidth / 4;

                    byte[] ybuff = new byte[ybufsize];
                    Buffer.BlockCopy(targetbuf, 0, ybuff, 0, ybufsize);

                    byte[] ubuff = new byte[ubufsize];
                    Buffer.BlockCopy(targetbuf, ybufsize, ubuff, 0, ubufsize);

                    byte[] vbuff = new byte[vbufsize];
                    Buffer.BlockCopy(targetbuf, ybufsize + ubufsize, vbuff, 0, vbufsize);

                    if (mYTexture != null)
                        Object.DestroyImmediate(mYTexture);
                    if (mUTexture != null)
                        Object.DestroyImmediate(mUTexture);
                    if (mVTexture != null)
                        Object.DestroyImmediate(mVTexture);

                    mYTexture = new Texture2D(videoWidth, videoHeight, TextureFormat.R8, false);
                    mUTexture = new Texture2D(videoWidth / 2, videoHeight / 2, TextureFormat.R8, false);
                    mVTexture = new Texture2D(videoWidth / 2, videoHeight / 2, TextureFormat.R8, false);

                    mYTexture.LoadRawTextureData(ybuff);
                    mUTexture.LoadRawTextureData(ubuff);
                    mVTexture.LoadRawTextureData(vbuff);

                    mYTexture.Apply(false);
                    mUTexture.Apply(false);
                    mVTexture.Apply(false);

                    mBlitYUVToRGBMat.SetTexture("_UTex", mUTexture);
                    mBlitYUVToRGBMat.SetTexture("_VTex", mVTexture);
                    mBlitYUVToRGBMat.SetMatrix("_YUVMat", _YUVToRGBMatrix);

                    RenderTexture pre = RenderTexture.active;
                    RenderTexture.active = _renderTexture;
                    Graphics.Blit(mYTexture, _renderTexture, mBlitYUVToRGBMat);
                    _targetRawImageToShowFrame.texture = _renderTexture;
                    _targetRawImageToShowFrame.SetNativeSize();
                    RenderTexture.active = pre;

                    LibVideoPlayerExport.player_getOneFrameBuffer_Done(buffptr);
                }
            }

            _CurFrameCount++;
            yield return new WaitForSeconds(_timeinterval);
            _curPlayTime += _timeinterval;
        }
    }
}