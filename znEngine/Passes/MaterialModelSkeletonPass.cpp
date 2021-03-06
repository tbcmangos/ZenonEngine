#include "stdafx.h"

// General
#include "MaterialModelSkeletonPass.h"

// Additional
#include "Materials/MaterialModel.h"

CMaterialModelSkeletonPass::CMaterialModelSkeletonPass(IRenderDevice& RenderDevice, std::shared_ptr<IScene> Scene)
	: Base3DPass(RenderDevice, Scene)
{
	
}

CMaterialModelSkeletonPass::~CMaterialModelSkeletonPass()
{}


IShaderParameter * CMaterialModelSkeletonPass::GetLightsShaderParameter() const
{
	return m_ShaderLightsBufferParameter;
}

IShaderParameter * CMaterialModelSkeletonPass::GetBonesShaderParameter() const
{
	return m_ShaderBonesBufferParameter;
}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CMaterialModelSkeletonPass::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	std::shared_ptr<IShader> vertexShader;
	std::shared_ptr<IShader> pixelShader;

	if (GetRenderDevice().GetDeviceType() == RenderDeviceType::RenderDeviceType_DirectX11)
	{
		vertexShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::VertexShader, "IDB_SHADER_3D_MODEL_FORWARD", "VS_main_Bones");
		pixelShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::PixelShader, "IDB_SHADER_3D_MODEL_FORWARD", "PS_main");
	}
	vertexShader->LoadInputLayoutFromReflector();

	// PIPELINES
	auto Pipeline = GetRenderDevice().GetObjectsFactory().CreatePipelineState();
	Pipeline->GetBlendState()->SetBlendMode(disableBlending);
	Pipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	Pipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	Pipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid, IRasterizerState::FillMode::Solid);
	Pipeline->SetRenderTarget(RenderTarget);
	Pipeline->SetShader(EShaderType::VertexShader, vertexShader);
	Pipeline->SetShader(EShaderType::PixelShader, pixelShader);

	auto sampler = GetRenderDevice().GetObjectsFactory().CreateSamplerState();
	sampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	sampler->SetWrapMode(ISamplerState::WrapMode::Repeat, ISamplerState::WrapMode::Repeat);
	Pipeline->SetSampler(0, sampler);

	auto samplerClamp = GetRenderDevice().GetObjectsFactory().CreateSamplerState();
	samplerClamp->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	samplerClamp->SetWrapMode(ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp);
	Pipeline->SetSampler(1, samplerClamp);

	m_ShaderBonesBufferParameter = &vertexShader->GetShaderParameterByName("Bones");
	_ASSERT(m_ShaderBonesBufferParameter->IsValid());

	m_ShaderLightsBufferParameter = &pixelShader->GetShaderParameterByName("Lights");
	_ASSERT(m_ShaderLightsBufferParameter->IsValid());

	return SetPipeline(Pipeline);
}



EVisitResult CMaterialModelSkeletonPass::Visit(const ISceneNode3D * SceneNode)
{
	auto skeletonComponent = SceneNode->GetComponent<ISkeletonComponent3D>();
	if (skeletonComponent == nullptr)
		return EVisitResult::AllowVisitChilds;

	m_ShaderBonesBufferParameter->Set(skeletonComponent->GetBonesBuffer());
	return Base3DPass::Visit(SceneNode);
}

//
// IVisitor
//
EVisitResult CMaterialModelSkeletonPass::Visit(const IModel * Model)
{
	return Base3DPass::Visit(Model);
}

EVisitResult CMaterialModelSkeletonPass::Visit(const IGeometry * Geometry, const IMaterial* Material, SGeometryDrawArgs GeometryDrawArgs)
{
	const MaterialModel* objMaterial = dynamic_cast<const MaterialModel*>(Material);
	if (objMaterial == nullptr)
		return EVisitResult::Block;

	//if (objMaterial->GetTexture(0) == nullptr || objMaterial->GetTexture(0)->IsTransparent() == true)
	//	return false;
	if (Material)
		Material->Bind(GetRenderEventArgs().PipelineState->GetShaders());

	m_ShaderLightsBufferParameter->Bind();
	m_ShaderBonesBufferParameter->Bind();

	Geometry->Render(GetRenderEventArgs(), GetRenderEventArgs().PipelineState->GetShaders().at(EShaderType::VertexShader).get(), GeometryDrawArgs);

	m_ShaderBonesBufferParameter->Unbind();
	m_ShaderLightsBufferParameter->Unbind();

	if (Material)
		Material->Unbind(GetRenderEventArgs().PipelineState->GetShaders());

	return AllowAll;
}
