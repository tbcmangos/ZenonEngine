#pragma once

enum OW_ENGINE_API RenderDeviceType
{
	RenderDeviceType_None = 0,
	RenderDeviceType_DirectX,
	RenderDeviceType_OpenGL
};

struct OW_ENGINE_API IznRenderDeviceCreator 
{
	virtual ~IznRenderDeviceCreator() {}

	virtual RenderDeviceType GetRenderDeviceType() const = 0;
	virtual std::shared_ptr<IRenderDevice> CreateRenderDevice() = 0;
};

struct OW_ENGINE_API 
	__declspec(uuid("E0313DD4-A169-4369-BA8C-2F10323832CB"))
	IznRenderDeviceCreatorFactory : IManager
{
	virtual ~IznRenderDeviceCreatorFactory() {}

	virtual void RegisterRenderDeviceCreator(std::shared_ptr<IznRenderDeviceCreator> RenderDeviceCreator) = 0;
	virtual void UnregisterRenderDeviceCreator(std::shared_ptr<IznRenderDeviceCreator> RenderDeviceCreator) = 0;
	virtual std::shared_ptr<IznRenderDeviceCreator> GetRenderDeviceCreator(RenderDeviceType RenderDeviceType) const = 0;
};