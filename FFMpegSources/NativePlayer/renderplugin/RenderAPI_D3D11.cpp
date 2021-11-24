#include "RenderAPI.h"
#include "PlatformBase.h"

#if SUPPORT_D3D11

#include <assert.h>
#include <d3d11.h>
#include "IUnityGraphicsD3D11.h"


class RenderAPI_D3D11 : public RenderAPI
{
public:
	RenderAPI_D3D11();
	virtual ~RenderAPI_D3D11() { }
	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);
	virtual void* BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch);
	virtual void EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr);
	virtual void ModifyTextureWithInputData(void* textureHandle, int width, int height, int rowPitch, void* dataPtr);
private:
	void CreateResources();
	void ReleaseResources();

private:
	ID3D11Device* m_Device;
};


RenderAPI* CreateRenderAPI_D3D11()
{
	return new RenderAPI_D3D11();
}


RenderAPI_D3D11::RenderAPI_D3D11() : m_Device(NULL)
{
}

void RenderAPI_D3D11::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	switch (type)
	{
	case kUnityGfxDeviceEventInitialize:
	{
		IUnityGraphicsD3D11* d3d = interfaces->Get<IUnityGraphicsD3D11>();
		m_Device = d3d->GetDevice();
		CreateResources();
		break;
	}
	case kUnityGfxDeviceEventShutdown:
		ReleaseResources();
		break;
	}
}

void RenderAPI_D3D11::CreateResources()
{

}

void RenderAPI_D3D11::ReleaseResources()
{

}

void* RenderAPI_D3D11::BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch)
{
	const int rowPitch = textureWidth * 4;
	// Just allocate a system memory buffer here for simplicity
	unsigned char* data = new unsigned char[rowPitch * textureHeight];
	*outRowPitch = rowPitch;
	return data;
}

void RenderAPI_D3D11::EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr)
{
	ID3D11Texture2D* d3dtex = (ID3D11Texture2D*)textureHandle;
	assert(d3dtex);

	ID3D11DeviceContext* ctx = NULL;
	m_Device->GetImmediateContext(&ctx);
	// Update texture data, and free the memory buffer
	ctx->UpdateSubresource(d3dtex, 0, NULL, dataPtr, rowPitch, 0);
	delete[](unsigned char*)dataPtr;
	ctx->Release();
}

void RenderAPI_D3D11::ModifyTextureWithInputData(void* textureHandle, int width, int height, int rowPitch, void* dataPtr)
{
	ID3D11Texture2D* d3dtex = (ID3D11Texture2D*)textureHandle;
	if (d3dtex != NULL && dataPtr != NULL)
	{
		ID3D11DeviceContext* ctx = NULL;
		m_Device->GetImmediateContext(&ctx);
		ctx->UpdateSubresource(d3dtex, 0, NULL, dataPtr, rowPitch, 0);
		ctx->Release();
	}
}

#endif