# Simple Video Render in Unity3d
render video in unity3d  
using ffmpeg c++ for video decoding

**unstable dev now  
self project to learn **


# How to make a simple video player in Unity3d

1. load video data by ffmpeg or other video decode tools to decode video file get video informations(fps, duration, format type ...)
2. use ffmpeg or other tools to decode video data to frame image format(RGB,YUV420,NV12 ...)
4. pass one frame data buffer from native player to unity3d (normal way)
5. unity3d convert native frame data buffer to byte array(like Inptr to byte array)
5. unity3d side need to create texture and updating texture with bytes
6. unity3d side we should convert frame buffer data to RGB color if input frame data isn't RGB format(like Y,UV to RGB)

# Video decode

in native side create a very simple ffmpeg c++ player to decode video frame data(audio not included),export interface to unity3d ,so we init player and decode video by unity3d side call.

# Texture Updating

> the key: we use yuv three textures(TextureFormat.R8) to store yuv plane value, and convert yuv to RGB with shader, so the most important of render video in unity3d is texture-updating, like we need update yuv textures each frame


we have multi-ways to make texture updating

## Using "CPU" normal way pass raw frame buffer data to unity3d (RGB, YUV...)
native side decode video data and pass frame buffer to unity3d, each frame we need to convert given IntPtr to bytes[] 
unity3d side will create texture2d with bytes or just update texture2d with buffer bytes

we has multi ways to make texture2d updating with input bytes

- 1 way---->. every frame to create new texture2d with input buffer by LoadRawTextureData, and destory pre-created texture2d
- 2 way---->. create texture2d once,everty frame we using SetPixelData to update texture2d's data

after update texture raw data with raw bytes, we need call Texture2D.Apply() upload texture to GPU finish texture-updating

Pros: 
1. freely to use different native output buffers (nv12, yuv420,rgb...)

Cons:  
1. pass buffer between native and unity3d will cause everyframe's GC  
2. not fully use GPU's power



## Using "GPU" - using opengl,DX...backend
unity3d side create textures by input video height , width and image format, use GetNativeTexturePtr() get texture2d's native ptr pass to native-render-backend, each frame we using opengl or dx to updating texture data

```

opengl ways:
1. update texture buffer glTexSubImage2D or glTexImage2D to update unity3d's texture in native opengl backend
2. if we using SurfaceTexture in Android, we can sample SurfaceTexture to fbo -> convert GL_TEXTURE_EXTERNAL_OES to GL_TEXTURE -> update unity3d's texture



```
https://github.com/Unity-Technologies/NativeRenderingPlugin


there is no buffer pass and convert between Native and Unity3d so will not GC happened, it's fater then using CPU texture update


## Using "GPU" - using CommandBuffer.IssuePluginCustomTextureUpdateV2

Use CommandBuffer.IssuePluginCustomTextureUpdateV2 Send a texture update event to a native code plugin.so there no need to create different render-api-backend to update texture, it's a common way.

native side
```
if (eventID == kUnityRenderingExtEventUpdateTextureBeginV2)
{
	// get one frame data and set data to texture
	// ...
	UnityRenderingExtTextureUpdateParamsV2 *params = (UnityRenderingExtTextureUpdateParamsV2 *)data;
	params->texData = framedata;
	// ...
}
else if (eventID == kUnityRenderingExtEventUpdateTextureEndV2)
{
	// release frame data after unity3d update texture end
	free framedata;
}
```

unity side
```

// each frame use command buffer to update texture like(Y,U,V)
_command.IssuePluginCustomTextureUpdateV2(callBack,
    mYTexture, (uint)0);
_command.IssuePluginCustomTextureUpdateV2(callBack,
    mUTexture, (uint)1);
_command.IssuePluginCustomTextureUpdateV2(callBack,
    mVTexture, (uint)2);
Graphics.ExecuteCommandBuffer(_command);
_command.Clear();

// with yuv textures using shader to output RGB color
renderRGBWithShader();

```

# Road

**we render video frame image to yuv420 format, nativeplayer will output yuv420 frame buffer.**
unity3d side we create three textures(YUV) , use shader sample yuv textures and with formula to outout RGB color  
we store yuv plane value in texture's **R** Channel so our debug yuv texture will looks RED  

```
YTexture : TextureFormat.R8  y in R Channel
UTexture : TextureFormat.R8  u in R Channel
VTexture : TextureFormat.R8  v in R Channel
```

## Dev Status
1. CPU-load buffer to update yuv texture each frame(Done)
2. GPU using command buffer (Done)
3. RenderAPI BackEnd (TODO)

# How to Run Samples
if not modify native-simple-player C++ and rebulid lib, just open scenes in unity editor

1. VideoPlayer-CommandBuffer scene(GPU command buffer)
2. VideoPlayer-CPU.unity(normal CPU)


# How to modify and build videoPlayerLib library

## install ffmpeglib-win

create 'ffmpeglib-win' folder at simplevideodemo/FFMpegSources/ place ffmpeg c++ windows library to this 'ffmpeglib-win' folder


## make projects (visual studio)

```
# UNIX Makefile
cmake ..
# Mac OSX
cmake -G "Xcode" ..

# Microsoft Windows
visual studio 2017
cmake -G "Visual Studio 15" ..
cmake -G "Visual Studio 15 Win64" ..
```

## build


# Screen Shot
![image](https://user-images.githubusercontent.com/14041295/142991396-cf7ddb0f-3de4-4e08-833e-3ecb8f53e276.png)
![image](https://user-images.githubusercontent.com/14041295/142991586-12de23db-474a-47ee-8654-98516dc40c34.png)
![image](https://user-images.githubusercontent.com/14041295/142991860-fec8f3e6-27f2-46ea-84a1-bfcdbab2b709.png)


