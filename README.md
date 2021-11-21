# simplevideodemo
self learn video rendering

# ffmpeglib-Windows
ffmpeglib-win folder place ffmpeg sdk

# cmake 
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


# How to make a simple video player in Unity3d

1. load video data by ffmpeg or other decode tools
2. decode frame data to target format(RGB,YUV ...)
4. pass frame data to unity3d
5. unity3d side will convert to RGB color if input frame data isn't RGB format(like Y,UV to RGB)


# Texture Updating
## normal way-CPU pass raw frame buffer data to unity3d (RGB, YUV...)
decode video frame and pass buffer to unity3d C# IntPtr, each frame we need to convert IntPtr to bytes[]  
Unity3d side will create texture2d with bytes  

will create texture2d each time and delete pre-texture2d each frame


## "GPU" way using opengl,DX...texture updating
just create once texture2d with width and height,pass to native-render-backend each frame just updating texture data, so there is no need to pass frame buffer data to Unity3d  
it's will be more effective and less memory usage.

