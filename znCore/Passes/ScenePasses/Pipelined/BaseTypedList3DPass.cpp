#include "stdafx.h"

// General
#include "BaseTypedList3DPass.h"

CBaseList3DPass::CBaseList3DPass(IRenderDevice& RenderDevice, const std::shared_ptr<CSceneCreateTypedListsPass>& SceneNodeListPass, SceneNodeType SceneNodeType)
	: RenderPassPipelined(RenderDevice)
	, m_SceneNodeListPass(SceneNodeListPass)
	, m_PerObjectParameter(nullptr)
{
	m_AcceptSceneNodeTypes.push_back(SceneNodeType);
	m_PerObjectConstantBuffer = GetRenderDevice().GetObjectsFactory().CreateConstantBuffer(PerObject3D());
}

CBaseList3DPass::CBaseList3DPass(IRenderDevice & RenderDevice, const std::shared_ptr<CSceneCreateTypedListsPass>& SceneNodeListPass, std::vector<SceneNodeType> SceneNodeTypesList)
	: RenderPassPipelined(RenderDevice)
	, m_SceneNodeListPass(SceneNodeListPass)
	, m_PerObjectParameter(nullptr)
{
	m_AcceptSceneNodeTypes = SceneNodeTypesList;
	m_PerObjectConstantBuffer = GetRenderDevice().GetObjectsFactory().CreateConstantBuffer(PerObject3D());
}

CBaseList3DPass::~CBaseList3DPass()
{
}

//
// IRenderPassPipelined
//
void CBaseList3DPass::Render(RenderEventArgs & e)
{
	/*const auto& acceptableNodeTypesList = GetAcceptableNodeTypes();
	std::for_each(acceptableNodeTypesList.begin(), acceptableNodeTypesList.end(), [this](SceneNodeType acceptableNodeType) {
		if (GetSceneNodeListPass()->HasNodesList(acceptableNodeType))
		{
			const auto& list = GetSceneNodeListPass()->GetNodesList(acceptableNodeType);
			std::for_each(list.begin(), list.end(), [this](const CSceneCreateTypelessListPass::SNodeElement& SceneNodeElement) {
				if (Visit(SceneNodeElement.SceneNode))
				{
					const auto& components = SceneNodeElement.SceneNode->GetComponents();
					std::for_each(components.begin(), components.end(), [this](const std::pair<GUID, std::shared_ptr<ISceneNodeComponent>>& Component) {
						Component.second->Accept(this);
					});
				}
			});
		}
	});*/

	for (const auto& acceptableNodeType : GetAcceptableNodeTypes())
	{
		if (GetSceneNodeListPass()->HasNodesList(acceptableNodeType))
		{
			for (const auto& SceneNodeElement : GetSceneNodeListPass()->GetNodesList(acceptableNodeType))
			{
				if (Visit(SceneNodeElement.SceneNode))
				{
					const auto& components = SceneNodeElement.SceneNode->GetComponents();
					std::for_each(components.begin(), components.end(), [this](const std::pair<GUID, std::shared_ptr<ISceneNodeComponent>>& Component) {
						Component.second->Accept(this);
					});
				}
			}
		}
	}
}

//
// IVisitor
//
bool CBaseList3DPass::Visit(const ISceneNode3D * SceneNode)
{
	static PerObject3D perObject3D;
	perObject3D.Model = SceneNode->GetWorldTransfom();
	m_PerObjectConstantBuffer->Set(perObject3D);

	if (m_PerObjectParameter == nullptr)
		m_PerObjectParameter = &(GetPipeline().GetShaders().at(EShaderType::VertexShader)->GetShaderParameterByName("PerObject"));

	if (m_PerObjectParameter->IsValid() && m_PerObjectConstantBuffer != nullptr)
	{
		m_PerObjectParameter->SetConstantBuffer(m_PerObjectConstantBuffer);
		m_PerObjectParameter->Bind();
	}

	return true;
}

bool CBaseList3DPass::Visit(const IModel * Model)
{
	return Model->Render(GetRenderEventArgs());
}

bool CBaseList3DPass::Visit(const IGeometry * Geometry, const IMaterial * Material, SGeometryDrawArgs GeometryDrawArgs)
{
	Material->Bind(GetRenderEventArgs().PipelineState->GetShaders());
	Geometry->Render(GetRenderEventArgs(), GetRenderEventArgs().PipelineState->GetShaders().at(EShaderType::VertexShader).get(), GeometryDrawArgs);
	Material->Unbind(GetRenderEventArgs().PipelineState->GetShaders());
	return true;
}



//
// Protected
//
const std::shared_ptr<CSceneCreateTypedListsPass>& CBaseList3DPass::GetSceneNodeListPass() const
{
	return m_SceneNodeListPass;
}

const std::vector<SceneNodeType>& CBaseList3DPass::GetAcceptableNodeTypes() const
{
	return m_AcceptSceneNodeTypes;
}
