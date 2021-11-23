# simplevideodemo
render video in unity3d  
using ffmpeg c++ for video decoding


# How to make a simple video player in Unity3d

1. load video data by ffmpeg or other video decode tools to decode video file get video informations(fps, duration, format type ...)
2. use ffmpeg or other tools to decode video frame data to image format(RGB,YUV420,NV120 ...)
4. pass one frame data buffer from native player to unity3d (normal way)
5. unity3d convert native frame data buffer to byte array(like Inptr to byte array)
5. unity3d side need to create texture and updating texture with bytes
6. unity3d side we should convert frame buffer data to RGB color if input frame data isn't RGB format(like Y,UV to RGB)

# Texture Updating

we have multi-ways to make texture updating

## "CPU" normal way pass raw frame buffer data to unity3d (RGB, YUV...)
decode video frame and pass buffer to unity3d, each frame we need to convert IntPtr to bytes[] 
unity3d side will create texture2d with bytes or just update texture2d with converted buffer bytes

we has multi ways to make texture2d updating with input bytes

- 1 way---->. every frame to create new texture2d with input buffer by LoadRawTextureData
- 2 way---->. just create texture2d once,everty frame we using SetPixelData to update texture2d's data

after update texture raw data with income bytes we need to use Texture2D.Apply() upload texture to GPU


Pros: 
1. freely to use different native output buffers (nv12, yuv420,rgb...)

Cons:  
1. pass buffer between native and unity3d will cause everyframe's GC  
2. not fully use GPU's power



## "GPU" - using opengl,DX...back end
unity3d side create textures by input video height and width, GetNativeTexturePtr() get native ptr pass to native-render-backend, each frame we using opengl or dx to updating texture data


there is not buffer pass and convert between Native and Unity3d so will not GC happened, it's fater then using CPU texture update


## "GPU" - using comamnd buffer (IssuePluginCustomTextureUpdateV2)

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
	delete framedata;
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


# How to Run


# How to install ffmpeglib-win

create 'ffmpeglib-win' folder at simplevideodemo/FFMpegSources/ place ffmpeg c++ windows library to this 'ffmpeglib-win' folder


# How to build videoPlayerLib lib

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


