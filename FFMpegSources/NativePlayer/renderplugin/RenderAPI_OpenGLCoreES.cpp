#include "RenderAPI.h"
#include "PlatformBase.h"
#include <iostream>

// OpenGL Core profile (desktop) or OpenGL ES (mobile) implementation of RenderAPI.
// Supports several flavors: Core, ES2, ES3

#if SUPPORT_OPENGL_UNIFIED


#include <assert.h>
#if UNITY_IOS || UNITY_TVOS
#	include <OpenGLES/ES2/gl.h>
#elif UNITY_ANDROID || UNITY_WEBGL
#	include <GLES2/gl2.h>
#elif UNITY_OSX
#	include <OpenGL/gl3.h>
#elif UNITY_WIN
// On Windows, use gl3w to initialize and load OpenGL Core functions. In principle any other
// library (like GLEW, GLFW etc.) can be used; here we use gl3w since it's simple and
// straightforward.
#	include "gl3w.h"

#elif UNITY_LINUX
#	define GL_GLEXT_PROTOTYPES
#	include <GL/gl.h>
#else
#	error Unknown platform
#endif

class RenderAPI_OpenGLCoreES : public RenderAPI
{
public:
	RenderAPI_OpenGLCoreES(UnityGfxRenderer apiType);
	virtual ~RenderAPI_OpenGLCoreES() { }
	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);
	virtual void* BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch);
	virtual void EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr);
	virtual void ModifyTextureWithInputData(void* textureHandle, int width, int height, int rowPitch, void* dataPtr);
private:
	void CreateResources();
private:
	UnityGfxRenderer m_APIType;
};

RenderAPI_OpenGLCoreES::RenderAPI_OpenGLCoreES(UnityGfxRenderer apiType) : m_APIType(apiType)
{
	std::cout << "RenderAPI_OpenGLCoreES() Constructor" << std::endl;
}

void RenderAPI_OpenGLCoreES::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	if (type == kUnityGfxDeviceEventInitialize)
	{
		CreateResources();
	}
	else if (type == kUnityGfxDeviceEventShutdown)
	{
		//@TODO: release resources
	}
}

//
// Create OpenGL Content
// 1. shader program
// 2. if opengl core we need to init gl3w https://github.com/skaslev/gl3w
void RenderAPI_OpenGLCoreES::CreateResources()
{
	// Create shaders
	if (m_APIType == kUnityGfxRendererOpenGLES20)
	{
		/*m_VertexShader = CreateShader(GL_VERTEX_SHADER, kGlesVProgTextGLES2);
		m_FragmentShader = CreateShader(GL_FRAGMENT_SHADER, kGlesFShaderTextGLES2);*/
	}
	else if (m_APIType == kUnityGfxRendererOpenGLES30)
	{
		//m_VertexShader = CreateShader(GL_VERTEX_SHADER, kGlesVProgTextGLES3);
		//m_FragmentShader = CreateShader(GL_FRAGMENT_SHADER, kGlesFShaderTextGLES3);
	}
#	if SUPPORT_OPENGL_CORE
	else if (m_APIType == kUnityGfxRendererOpenGLCore)
	{
#		if UNITY_WIN
		int ret = gl3wInit();
		if (ret)
		{
			std::cout << "RenderAPI_OpenGLCoreES()::CreateResources() gl3wInit() Fatal Error" << std::endl;
		}
#		endif

		//m_VertexShader = CreateShader(GL_VERTEX_SHADER, kGlesVProgTextGLCore);
		//m_FragmentShader = CreateShader(GL_FRAGMENT_SHADER, kGlesFShaderTextGLCore);
	}
#	endif // if SUPPORT_OPENGL_CORE
}

void* RenderAPI_OpenGLCoreES::BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch)
{
	const int rowPitch = textureWidth * 4;
	unsigned char* data = new unsigned char[rowPitch * textureHeight];
	*outRowPitch = rowPitch;
	return data;
}

void RenderAPI_OpenGLCoreES::EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr)
{
	GLuint gltex = (GLuint)(size_t)(textureHandle);
	// Update Texture Data,and free the memory buffer
	glBindTexture(GL_TEXTURE_2D, gltex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, GL_RGBA, GL_UNSIGNED_BYTE, dataPtr);
	delete[](unsigned char*)dataPtr;
}

// Save data in R8 Format
void RenderAPI_OpenGLCoreES::ModifyTextureWithInputData(void* textureHandle, int width, int height, int rowPitch, void* dataPtr)
{
	if (textureHandle != NULL && dataPtr != NULL)
	{
		GLuint gltex = (GLuint)(size_t)(textureHandle);
		glBindTexture(GL_TEXTURE_2D, gltex);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, dataPtr);
	}
}

RenderAPI* CreateRenderAPI_OpenGLCoreES(UnityGfxRenderer apiType)
{
	return new RenderAPI_OpenGLCoreES(apiType);
}

#endif // #if SUPPORT_OPENGL_UNIFIED
