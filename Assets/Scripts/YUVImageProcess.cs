using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using UnityEngine.Networking;
using System.IO;
using System.Diagnostics;
using Debug = UnityEngine.Debug;

/*
 * 
 */

public class YUVImageProcess : MonoBehaviour
{
    // YUV image text
    public string ybufferFile;
    public string ubufferFile;
    public string vbufferFile;

    private string TAG = "YUVImageProcess";

    // Y buffer to image
    // UV buffer can merge to one file only one sampler
    // or just split to two image (U, V) ?? 
    public RawImage _targetRawImageToShowFrame;

    public MeshRenderer _meshRender;

    // frame width and height
    public int _frameWidth = 100;
    public int _frameHeight = 100;
    public Shader _i420toRGB;

    // YUV Texture
    private Texture2D mYTexture;
    private Texture2D mUTexture;
    private Texture2D mVTexture;

    // RGB Texture
    private RenderTexture mRGBTexture;
    public Material mBlitYUVToRGBMat;

    private Matrix4x4 _YUVToRGBMatrix;

    // Start is called before the first frame update
    void Start()
    {
        _YUVToRGBMatrix.SetRow(0, new Vector4(1, 0, 1.4022f, 0));
        _YUVToRGBMatrix.SetRow(1, new Vector4(1, -0.3456f, -0.7145f, 0));
        _YUVToRGBMatrix.SetRow(2, new Vector4(1, 1.771f, 0, 0));
        _YUVToRGBMatrix.SetRow(3, new Vector4(0, 0, 0, 1));
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Space))
        {
            // ConvertYUVToRGB();
            BlitYUVToRGB();
        }

        if (Input.GetKeyDown(KeyCode.Y))
        {
            LoadSingleChannel(YUV_TYPE.Y);
        }
        else if (Input.GetKeyDown(KeyCode.U))
        {
            LoadSingleChannel(YUV_TYPE.U);
        }
        else if (Input.GetKeyDown(KeyCode.V))
        {
            LoadSingleChannel(YUV_TYPE.V);
        }
    }

    private enum YUV_TYPE
    {
        Y,
        U,
        V,
        UV
    }

    private Texture2D LoadSingleChannel(YUV_TYPE inputType)
    {
        string path = string.Empty;
        int width = _frameWidth;
        int height = _frameHeight;
        switch (inputType)
        {
            case YUV_TYPE.Y:
                path = Path.Combine(Application.dataPath, "Data", "frame-1-y.origin");
                break;
            case YUV_TYPE.U:
                path = Path.Combine(Application.dataPath, "Data", "frame-1-u.origin");
                width /= 2;
                height /= 2;
                break;
            case YUV_TYPE.V:
                path = Path.Combine(Application.dataPath, "Data", "frame-1-v.origin");
                width /= 2;
                height /= 2;
                break;
            case YUV_TYPE.UV:
                width /= 2;
                height /= 2;
                path = Path.Combine(Application.dataPath, "Data", "frame-1-u.origin");
                byte[] uBytes = File.ReadAllBytes(path);
                path = Path.Combine(Application.dataPath, "Data", "frame-1-v.origin");
                byte[] vBytes = File.ReadAllBytes(path);

                Debug.Log("ubytes:" + uBytes.Length);
                Debug.Log("vbytes:" + vBytes.Length);

                Texture2D uv = new Texture2D(width, height, TextureFormat.RG16, false);
                uv.wrapMode = TextureWrapMode.Clamp;
                byte[] totalBytes = new byte[uBytes.Length + vBytes.Length];
                System.Buffer.BlockCopy(uBytes, 0, totalBytes, 0, uBytes.Length);
                System.Buffer.BlockCopy(vBytes, 0, totalBytes, uBytes.Length, vBytes.Length);
                uv.LoadRawTextureData(totalBytes);
                uv.Apply(false);
                return uv;
                break;
            default:
                break;
        }

        Debug.Log("LoadSingleChannel : " + inputType.ToString() + ",width:" + width + ",height:" + height);

        byte[] buffer = File.ReadAllBytes(path);
        Texture2D texture = new Texture2D(width, height, TextureFormat.R8, false);
        texture.LoadRawTextureData(buffer);
        texture.Apply(false);
        return texture;
    }

    // private void ConvertYUVToRGB()
    // {
    //     SimpleDebuger.LogInfo(TAG, "Start");
    //
    //     Texture2D y = LoadSingleChannel(YUV_TYPE.Y);
    //     Texture2D u = LoadSingleChannel(YUV_TYPE.U);
    //     Texture2D v = LoadSingleChannel(YUV_TYPE.V);
    //
    //     _meshRender.sharedMaterial.SetTexture("_UTex", u);
    //     _meshRender.sharedMaterial.SetTexture("_VTex", v);
    //     _meshRender.sharedMaterial.SetMatrix("_YUVMat", _YUVToRGBMatrix);
    //
    //     SimpleDebuger.LogInfo(TAG, "End");
    // }

    private void BlitYUVToRGB()
    {
        Stopwatch sw = new Stopwatch();
        sw.Start();
        Texture2D y = LoadSingleChannel(YUV_TYPE.Y);
        Texture2D u = LoadSingleChannel(YUV_TYPE.UV);
        Texture2D v = LoadSingleChannel(YUV_TYPE.UV);

        bool needcreate = true;
        if (mRGBTexture != null && mRGBTexture.width == _frameWidth && mRGBTexture.height == _frameHeight)
        {
            needcreate = false;
        }

        if (needcreate)
        {
            mRGBTexture = new RenderTexture(_frameWidth, _frameHeight, 0, RenderTextureFormat.ARGB32);
        }

        mBlitYUVToRGBMat.SetTexture("_UTex", u);
        mBlitYUVToRGBMat.SetTexture("_VTex", v);
        mBlitYUVToRGBMat.SetMatrix("_YUVMat", _YUVToRGBMatrix);

        RenderTexture pre = RenderTexture.active;
        RenderTexture.active = mRGBTexture;
        // no need to GL.Clear()
        // GL.Clear(true, true, Color.black);
        Graphics.Blit(y, mRGBTexture, mBlitYUVToRGBMat);
        RenderTexture.active = pre;
        _targetRawImageToShowFrame.texture = mRGBTexture;
        _targetRawImageToShowFrame.SetNativeSize();

        sw.Stop();
        Debug.Log("blit done take time : " + sw.ElapsedMilliseconds + "(ms)");
    }
}