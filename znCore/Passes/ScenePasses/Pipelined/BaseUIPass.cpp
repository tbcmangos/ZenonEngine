#include "stdafx.h"

// General
#include "BaseUIPass.h"

BaseUIPass::BaseUIPass(IRenderDevice& RenderDevice, const std::shared_ptr<IScene>& Scene)
	: ScenePassPipelined(RenderDevice, Scene)
	, m_PerObjectParameter(nullptr)
{
	m_PerObjectConstantBuffer = GetRenderDevice().GetObjectsFactory().CreateConstantBuffer(PerObject());
}

BaseUIPass::~BaseUIPass()
{
}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> BaseUIPass::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	std::shared_ptr<IPipelineState> UIPipeline = GetRenderDevice().GetObjectsFactory().CreatePipelineState();
	UIPipeline->GetBlendState()->SetBlendMode(alphaBlending);
	UIPipeline->GetDepthStencilState()->SetDepthMode(disableDepthWrites);
	UIPipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	UIPipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid, IRasterizerState::FillMode::Solid);
	UIPipeline->GetRasterizerState()->SetAntialiasedLineEnable(false);
	UIPipeline->GetRasterizerState()->SetMultisampleEnabled(false);
	UIPipeline->SetRenderTarget(RenderTarget);

	return SetPipeline(UIPipeline);
}



//
// IVisitor
//
EVisitResult BaseUIPass::Visit(const ISceneNodeUI* sceneNode)
{
	PerObject perObject;
	perObject.Model = sceneNode->GetWorldTransfom();
	m_PerObjectConstantBuffer->Set(perObject);

	if (m_PerObjectParameter == nullptr)
		m_PerObjectParameter = &(GetPipeline().GetShaders().at(EShaderType::VertexShader)->GetShaderParameterByName("PerObject"));

	if (m_PerObjectParameter->IsValid() && m_PerObjectConstantBuffer != nullptr)
	{
		m_PerObjectParameter->SetConstantBuffer(m_PerObjectConstantBuffer);
		m_PerObjectParameter->Bind();
	}

	return EVisitResult::AllowAll;
}

EVisitResult BaseUIPass::Visit(const IModel * Model)
{
	Model->Render(GetRenderEventArgs());
	return EVisitResult::AllowAll;
}

EVisitResult BaseUIPass::Visit(const IGeometry* Geometry, const IMaterial* Material, SGeometryDrawArgs GeometryDrawArgs)
{
	Material->Bind(GetRenderEventArgs().PipelineState->GetShaders());
	Geometry->Render(GetRenderEventArgs(), GetRenderEventArgs().PipelineState->GetShaders().at(EShaderType::VertexShader).get(), GeometryDrawArgs);
	Material->Unbind(GetRenderEventArgs().PipelineState->GetShaders());

	return EVisitResult::AllowAll;
}



//
// Protected
//
void BaseUIPass::FillPerFrameData()
{
	const Viewport& viewport = GetRenderEventArgs().PipelineState->GetRenderTarget()->GetViewport();

	PerFrame perFrame(
		glm::mat4(1.0f), 
		viewport.GetOrthoMatix(), 
		glm::vec2(viewport.GetWidth(), viewport.GetHeight())
	);
	SetPerFrameData(perFrame);
}
