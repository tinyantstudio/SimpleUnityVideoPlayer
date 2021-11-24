#include "IUnityInterface.h"
#include "IUnityGraphics.h"
#include "RenderAPI.h"
#include <assert.h>
#include <iostream>
#include <string>

#include "videoPlayerExport.h"

static std::string TAG = "RenderAPI_Export: ";

static void* g_TextureHandle_Y = NULL;
static void* g_TextureHandle_U = NULL;
static void* g_TextureHandle_V = NULL;

static int   g_TextureWidth_Y = 0;
static int   g_TextureHeight_Y = 0;

static int   g_TextureHeight_UV = 0;
static int   g_TextureWidth_UV = 0;

static bool g_IsValidToRenderVideo = false;

static float g_Time;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetTimeFromUnity(float t)
{
	g_Time = t;
}

// Example for Texture Update plasma effect
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetTextureFromUnity(void* texture, int width, int height)
{
	g_TextureHandle_Y = texture;
	g_TextureHeight_Y = height;
	g_TextureWidth_Y = width;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetYUVTextureFromUnity(void* ytexture, void* utexture, void* vtexture, int yw, int yh, int uvw, int uvh)
{
	std::cout << TAG << "SetTextureFromUnity()" << std::endl;
	// A script calls this at initialization time; just remember the texture pointer here.
	// Will update texture pixels each frame from the plugin rendering event (texture update
	// needs to happen on the rendering thread).
	g_TextureHandle_Y = ytexture;
	g_TextureHandle_U = utexture;
	g_TextureHandle_V = vtexture;

	g_TextureWidth_Y = yw;
	g_TextureHeight_Y = yh;
	g_TextureHeight_UV = uvh;
	g_TextureWidth_UV = uvw;
}

static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);
extern "C" void	UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
	std::cout << TAG << "UnityPluginLoad()" << std::endl;
	s_UnityInterfaces = unityInterfaces;
	s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
	s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

#if SUPPORT_VULKAN
	if (s_Graphics->GetRenderer() == kUnityGfxRendererNull)
	{
		extern void RenderAPI_Vulkan_OnPluginLoad(IUnityInterfaces*);
		RenderAPI_Vulkan_OnPluginLoad(unityInterfaces);
	}
#endif // SUPPORT_VULKAN

	// Run OnGraphicsDeviceEvent(initialize) manually on plugin load
	OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
	std::cout << TAG << "UnityPluginUnload()" << std::endl;
	s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

static RenderAPI* s_CurrentAPI = NULL;
static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
	// Create graphics API implementation upon initialization
	if (eventType == kUnityGfxDeviceEventInitialize)
	{
		assert(s_CurrentAPI == NULL);
		s_DeviceType = s_Graphics->GetRenderer();
		s_CurrentAPI = CreateRenderAPI(s_DeviceType);
		std::cout << "OnGraphicsDeviceEvent() eventType:kUnityGfxDeviceEventInitialize deviceType: " << s_DeviceType << std::endl;
		if (s_CurrentAPI == NULL)
		{
			std::cout << "OnGraphicsDeviceEvent() eventType:kUnityGfxDeviceEventInitialize create RenderAPI Fatal Error" << std::endl;
		}
		else
		{
			std::cout << "OnGraphicsDeviceEvent() eventType:kUnityGfxDeviceEventInitialize create RenderAPI Suc" << std::endl;
		}
	}

	// Let the implementation process the device related events
	if (s_CurrentAPI)
	{
		s_CurrentAPI->ProcessDeviceEvent(eventType, s_UnityInterfaces);
	}

	// Cleanup graphics API implementation upon shutdown
	if (eventType == kUnityGfxDeviceEventShutdown)
	{
		delete s_CurrentAPI;
		s_CurrentAPI = NULL;
		s_DeviceType = kUnityGfxRendererNull;
	}
}

// --------------------------------------------------------------------------
// OnRenderEvent
// This will be called for GL.IssuePluginEvent script calls; eventID will
// be the integer passed to IssuePluginEvent. In this example, we just ignore
// that value.

// Example : update input texture by plasma effect
static void ModifyTexturePixels()
{
	// # debug
	// std::cout << "ModifyTexturePixels() called..." << std::endl;
	void* textureHandle = g_TextureHandle_Y;
	int width = g_TextureWidth_Y;
	int height = g_TextureHeight_Y;
	if (!textureHandle)
		return;

	int textureRowPitch;
	void* textureDataPtr = s_CurrentAPI->BeginModifyTexture(textureHandle, width, height, &textureRowPitch);
	if (!textureDataPtr)
		return;

	const float t = g_Time * 4.0f;

	unsigned char* dst = (unsigned char*)textureDataPtr;
	for (int y = 0; y < height; ++y)
	{
		unsigned char* ptr = dst;
		for (int x = 0; x < width; ++x)
		{
			// Simple "plasma effect": several combined sine waves
			int vv = int(
				(127.0f + (127.0f * sinf(x / 7.0f + t))) +
				(127.0f + (127.0f * sinf(y / 5.0f - t))) +
				(127.0f + (127.0f * sinf((x + y) / 6.0f - t))) +
				(127.0f + (127.0f * sinf(sqrtf(float(x*x + y * y)) / 4.0f - t)))
				) / 4;

			// Write the texture pixel
			ptr[0] = vv;
			ptr[1] = vv;
			ptr[2] = vv;
			ptr[3] = vv;

			// To next pixel (our pixels are 4 bpp)
			ptr += 4;
		}
		// To next image row
		dst += textureRowPitch;
	}
	s_CurrentAPI->EndModifyTexture(textureHandle, width, height, textureRowPitch, textureDataPtr);
}

static void ModifyYUVTextureWithVideoFrameData()
{
	if (g_IsValidToRenderVideo && s_CurrentAPI != NULL)
	{
		void* framedata_Y = NULL;
		void* framedata_U = NULL;
		void* framedata_V = NULL;

		framedata_Y = player_get_pop_y_buffer();
		framedata_U = player_get_pop_u_buffer();
		framedata_V = player_get_pop_v_buffer();

		const int rowPitchY = g_TextureWidth_Y * 1;
		const int rowPitchUV = g_TextureWidth_UV * 1;

		s_CurrentAPI->ModifyTextureWithInputData(g_TextureHandle_Y, g_TextureWidth_Y, g_TextureHeight_Y, rowPitchY, framedata_Y);
		s_CurrentAPI->ModifyTextureWithInputData(g_TextureHandle_U, g_TextureWidth_UV, g_TextureHeight_UV, rowPitchUV, framedata_U);
		s_CurrentAPI->ModifyTextureWithInputData(g_TextureHandle_V, g_TextureWidth_UV, g_TextureHeight_UV, rowPitchUV, framedata_V);

		// std::cout << TAG << "ModifyYUVTextureWithVideoFrameData() Update YUV Texture..." << std::endl;
	}
	else
	{
		// std::cout << TAG << "ModifyYUVTextureWithVideoFrameData() Skip..." << std::endl;
	}
}

// Define Own EventId start with 10
typedef enum CustomEventType
{
	kCustom_Update_YUV = 1 << 10,
	kCustom_VideoPlayerValid = 1 << 11,
	kCustom_VideoPlayerInvalid = 1 << 12
}CustomEventType;

// Unity Render Thread 
// Make native render here
static void UNITY_INTERFACE_API OnRenderEvent(int eventID)
{
	if (s_CurrentAPI == NULL)
		return;
	// Example
	// ModifyTexturePixels();

	if (kCustom_VideoPlayerValid == eventID)
	{
		g_IsValidToRenderVideo = true;
	}
	else if (kCustom_VideoPlayerInvalid == eventID)
	{
		g_IsValidToRenderVideo = false;
	}
	else if (kCustom_Update_YUV == eventID)
	{
		ModifyYUVTextureWithVideoFrameData();
	}
}

// --------------------------------------------------------------------------
// GetRenderEventFunc, an example function we export which is used to get a rendering event callback function.

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc()
{
	return OnRenderEvent;
}
