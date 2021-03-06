#pragma once

#include "Scene/SceneCreateTypelessListPass.h"

class CDefferedRender
	: public RenderPassPipelined
{
public:
	CDefferedRender(IRenderDevice& RenderDevice, const std::shared_ptr<CSceneCreateTypelessListPass>& SceneCreateTypelessListPass);
	virtual ~CDefferedRender();

	// CDefferedRender
	std::shared_ptr<ITexture> GetTexture0() const;
	std::shared_ptr<ITexture> GetTexture1() const;
	std::shared_ptr<ITexture> GetTexture2() const;
	std::shared_ptr<ITexture> GetTexture3() const;
	std::shared_ptr<ITexture> GetTextureDepthStencil() const;

	// IRenderPass
	void PreRender(RenderEventArgs& e) override;
	void Render(RenderEventArgs& e) override;

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override;

	// IVisitor
	EVisitResult Visit(const ISceneNode3D* node) override;
	EVisitResult Visit(const IGeometry* Geometry, const IMaterial* Material, SGeometryDrawArgs GeometryDrawArgs = SGeometryDrawArgs()) override;
	EVisitResult Visit(const ILight3D* light) override;

private:
	std::shared_ptr<CSceneCreateTypelessListPass> m_SceneCreateTypelessListPass;

private:
	std::shared_ptr<IConstantBuffer> m_PerObjectConstantBuffer;
	IShaderParameter* m_PerObjectShaderParameter;

private:
	std::shared_ptr<ITexture> m_Texture0;
	std::shared_ptr<ITexture> m_Texture1;
	std::shared_ptr<ITexture> m_Texture2;
	std::shared_ptr<ITexture> m_Texture3;
	std::shared_ptr<ITexture> m_DepthStencilTexture;
};