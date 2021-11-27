using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using System.Diagnostics;
using UnityEngine.Profiling;
using Debug = UnityEngine.Debug;
using System.IO;
using System.Text;
using System.Xml.Linq;
using UnityEditor;
using UnityEngine.Experimental.Rendering;
using UnityEngine.UI;

// How to read back texture back from GPU to CPU!!!
// 1. native method -> http://www.songho.ca/opengl/gl_pbo.html 
// 2. native method -> opengl,dx11,... 
// 3. unity's AsyncGPUReadback(Allows the asynchronous read back of GPU resources.)This class is used to copy resource data from the GPU to the CPU without any stall (GPU or CPU),
//    but adds a few frames of latency!!
//    https://docs.unity3d.com/ScriptReference/Rendering.AsyncGPUReadback.html  https://github.com/keijiro/AsyncCaptureTest
// 4. unity's ReadPixels() CPU and GPU stall but in same frame unity should wait unit this frame render finished
// 5. unity's CommandBuffer Compute Shader ? 


// use case 
// 1. make painting app just using texture2d in GPU don't read/write back from GPU and CPU

public class RecordCamera : MonoBehaviour
{
    //
    // Capture Camera
    // 

    public Camera mTargetCamera;
    public RawImage mRawImage;

    public void OnPostRender()
    {
        Debug.Log("post render...");
        // FirstWay();
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.A))
        {
            // StartCoroutine(_start());
            DoCapture(CaptureMethod.Default);
        }
        else if (Input.GetKeyDown(KeyCode.S))
        {
            DoCapture(CaptureMethod.Second);

        }
        else if (Input.GetKeyDown(KeyCode.D))
        {
            DoCapture(CaptureMethod.Third);
        }
    }

    public int captureWidth = 480;
    public int captureHeight = 640;

    public enum CaptureMethod
    {
        Default,
        First,
        Second,
        Third,
    }

    private void DoCapture(CaptureMethod method)
    {
        // Debug.Log("DoCapture() start: " + method);
        Stopwatch sw = new Stopwatch();
        sw.Start();
        Profiler.BeginSample("capture_first_way");
        if (method == CaptureMethod.Default)
            DefaultWay();
        if (method == CaptureMethod.First)
            FirstWay();
        else if (method == CaptureMethod.Second)
            SecondWay();
        else
            ThirdWay();

        Profiler.EndSample();
        sw.Stop();
        // Debug.Log("DoCapture() end: take time: " + sw.ElapsedMilliseconds.ToString() + "ms");
    }

    private void ThirdWay()
    {
    }

    private void SecondWay()
    {
    }

    private Texture2D _backUpTexture2D;
    private RenderTexture _backUpRenderTexture;

    private Texture2D CreateTexture2D(int width, int height, TextureFormat format)
    {
        bool needcreate = false;
        if (_backUpTexture2D == null || _backUpTexture2D.width != width || _backUpTexture2D.height != height)
            needcreate = true;
        if (needcreate)
        {
            if (_backUpTexture2D != null)
            {
                GameObject.Destroy(_backUpTexture2D);
            }

            _backUpTexture2D = new Texture2D(width, height, format, false);
            _backUpTexture2D.name = "Temp-Texture-2d";
            mRawImage.texture = _backUpTexture2D;
            Debug.Log("create new Texture2D");
        }
        return _backUpTexture2D;
    }

    private RenderTexture GetRenderTexture(int width, int height, RenderTextureFormat format)
    {
        bool needcreate = false;
        if (_backUpRenderTexture == null || _backUpRenderTexture.width != width || _backUpRenderTexture.height != height)
            needcreate = true;

        if (needcreate)
        {
            if (_backUpRenderTexture != null)
            {
                GameObject.Destroy(_backUpRenderTexture);
            }

            _backUpRenderTexture = new RenderTexture(width, height, 0, format, RenderTextureReadWrite.Default);
            _backUpRenderTexture.name = "Temp-RenderTexture";
            Debug.Log("create new RenderTexture");
        }
        return _backUpRenderTexture;
    }

    private void DefaultWay()
    {
        // make GPU/CPU stall when using ReadPixels() called (time line profiler mark:Gfx->ReadbackImage make GPU and CPU stall)
        // if not used in GPU not call Apply()
        // if used in GPU u should call Apply()
        // Apply() is expensive Upload texture data to GPU texture updating (time line profiler mark->Gfx.UploadTexture,Gfx.CreateTexture)
        // rawimage referenced texture is GPU Texture

        // if not used in CPU, just using Graphics.CopyTexture not use ReadPixels()
        // back up RenderTexture and Texture2D if same width height format
        // No GC

        // We using unity3d's Incremental GC to avoid large probability GC.Collect called when we use texture.GetPixels32() or texture.GetRawTextureData()
        // GC.Collect will take 2-3ms times

        // total time ReadPixels() 2.4-3.0ms
        // Camera.Render() 0.5ms
        // if use Color32ArrayToByteArray take 0.5ms but double buffer data size (Incremental GC no GC.Collect called)
        // if use GetRawTextureData take 0.5ms one buffer data size (Incremental GC no GC.Collect called)

        //
        // need waitforendofframe to readpixels from rendertexture
        // or just call Camera.Render() manually not recommend when video capture.
        // 

        RenderTexture rt = GetRenderTexture(captureWidth, captureHeight, RenderTextureFormat.ARGB32);
        mTargetCamera.targetTexture = rt;
        mTargetCamera.Render();

        RenderTexture.active = rt;
        Texture2D ss = CreateTexture2D(captureWidth, captureHeight, TextureFormat.ARGB32);
        Rect rc = new Rect(0, 0, captureWidth, captureHeight);
        // will take long time 2.5-3.5ms
        ss.ReadPixels(rc, 0, 0);

        // no need here if we not use ss in GPU
        // upload texture to GPU is expensive
        // https://docs.unity3d.com/ScriptReference/Texture2D.Apply.html
        // ss.Apply(false);
        mTargetCamera.targetTexture = null;
        RenderTexture.active = null;
    }

    private void FirstWay()
    {
        Debug.Log("SystemInfo.copyTextureSupport: " + SystemInfo.copyTextureSupport);
        RenderTexture preactive = RenderTexture.active;
        // RenderTexture rt = new RenderTexture(captureWidth, captureHeight, 0);
        RenderTexture rt = RenderTexture.GetTemporary(captureWidth, captureHeight, 0, RenderTextureFormat.ARGB32);
        mTargetCamera.targetTexture = rt;
        mTargetCamera.Render();

        RenderTexture.active = rt;
        Texture2D ss = CreateTexture2D(captureWidth, captureHeight, TextureFormat.ARGB32);
        Rect rc = new Rect(0, 0, captureWidth, captureHeight);
        ss.ReadPixels(rc, 0, 0);
        // so fast now
        // Graphics.CopyTexture(rt, ss);
        // ss.Apply();

        if (saveToLocal)
        {
            DealWithTexture2d(ss, CaptureMethod.First);
        }

        mTargetCamera.targetTexture = null;
        RenderTexture.active = preactive;
        // GameObject.Destroy(rt);
        //captureWidth = 480;
        //captureHeight = 640;
        RenderTexture.ReleaseTemporary(rt);
    }

    public bool saveToLocal = false;
    private string saveRootPath = string.Empty;

    private void DealWithTexture2d(Texture2D texture, CaptureMethod method)
    {
        // compare two method Color32ArrayToByteArray and GetRawTextureData
        // 测试的时候需要排除GC.Collect的干扰，这部分会占用一定的CPU时间
        Profiler.BeginSample("capture_first_way copy now...");
        //byte[] bt = Color32ArrayToByteArray(texture.GetPixels32());
        //byte[] bb = texture.GetRawTextureData();

        Profiler.EndSample();
        return;
        // Debug.Log("bytes raw length: " + bb.Length.ToString());

        //Debug.Log("bytes length: " + bt.Length.ToString());

        if (string.IsNullOrEmpty(saveRootPath))
        {
            saveRootPath = Path.Combine(Application.dataPath, "../outputframe");
            if (!Directory.Exists(saveRootPath))
                Directory.CreateDirectory(saveRootPath);
        }

        long time = DateTime.Now.ToFileTime();
        string savename = $"{method.ToString()}-{time}.png";
        string savename_bt = $"{method.ToString()}-{time}-bt.byte";
        string savename_bb = $"{method.ToString()}-{time}-bb.byte";
        string path = Path.Combine(saveRootPath, savename);
        byte[] savebytes = texture.EncodeToPNG();
        File.WriteAllBytes(path, savebytes);

        //path = Path.Combine(saveRootPath, savename_bt);
        //File.WriteAllBytes(path, bt);
        //path = Path.Combine(saveRootPath, savename_bb);
        //File.WriteAllBytes(path, bb);

        Debug.Log("save to local: " + path);
    }


    private static byte[] Color32ArrayToByteArray(Color32[] colors)
    {
        if (colors == null || colors.Length == 0)
            return null;

        int count = 0;
        foreach (var color32 in colors)
        {
            count++;
            if (count >= 100)
                break;
            Debug.Log("color: " + color32.ToString());
        }

        int lengthOfColor32 = Marshal.SizeOf(typeof(Color32));
        int length = lengthOfColor32 * colors.Length;
        byte[] bytes = new byte[length];

        GCHandle handle = default(GCHandle);
        try
        {
            handle = GCHandle.Alloc(colors, GCHandleType.Pinned);
            IntPtr ptr = handle.AddrOfPinnedObject();
            Marshal.Copy(ptr, bytes, 0, length);
        }
        finally
        {
            if (handle != default(GCHandle))
                handle.Free();
        }

        return bytes;
    }
}
