#include "RenderAPI.h"
#include "PlatformBase.h"

#if SUPPORT_D3D12

#include <assert.h>
#include <d3d12.h>
#include "IUnityGraphicsD3D12.h"

class RenderAPI_D3D12 : public RenderAPI
{
public:
	RenderAPI_D3D12();
	virtual ~RenderAPI_D3D12() { }
	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);
	virtual void* BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch);
	virtual void EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr);
	virtual void ModifyTextureWithInputData(void* textureHandle, int width, int height, int rowPitch, void* dataPtr);
private:
	void CreateResources();
	void ReleaseResources();

private:
	ID3D12Device* m_Device;
};


RenderAPI* CreateRenderAPI_D3D12()
{
	return new RenderAPI_D3D12();
}


RenderAPI_D3D12::RenderAPI_D3D12() : m_Device(NULL)
{
}

void RenderAPI_D3D12::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	switch (type)
	{
	case kUnityGfxDeviceEventInitialize:
	{
		IUnityGraphicsD3D12* d3d = interfaces->Get<IUnityGraphicsD3D12>();
		m_Device = d3d->GetDevice();
		CreateResources();
		break;
	}
	case kUnityGfxDeviceEventShutdown:
		ReleaseResources();
		break;
	}
}

void RenderAPI_D3D12::CreateResources()
{

}

void RenderAPI_D3D12::ReleaseResources()
{

}

void* RenderAPI_D3D12::BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch)
{
	const int rowPitch = textureWidth * 4;
	// Just allocate a system memory buffer here for simplicity
	unsigned char* data = new unsigned char[rowPitch * textureHeight];
	*outRowPitch = rowPitch;
	return data;
}

void RenderAPI_D3D12::EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr)
{
}

void RenderAPI_D3D12::ModifyTextureWithInputData(void* textureHandle, int width, int height, int rowPitch, void* dataPtr)
{
}

#endif