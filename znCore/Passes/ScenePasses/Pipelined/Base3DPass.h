#pragma once

#include "ScenePassPipelined.h"
#include "../SceneCreateTypedListsPass.h"

__declspec(align(16)) struct ZN_API PerObject3D
{
	glm::mat4 Model;
};


class ZN_API Base3DPass 
	: public ScenePassPipelined
{
public:
	Base3DPass(IRenderDevice& RenderDevice, const std::shared_ptr<IScene>& scene);
	virtual ~Base3DPass();

	// IVisitor
	virtual EVisitResult Visit(const ISceneNode3D* SceneNode) override;
	virtual EVisitResult Visit(const IModel* Model) override;
	virtual EVisitResult Visit(const IGeometry* Geometry, const IMaterial* Material, SGeometryDrawArgs GeometryDrawArgs = SGeometryDrawArgs()) override;

protected:
	std::shared_ptr<IConstantBuffer> m_PerObjectConstantBuffer;
	IShaderParameter* m_PerObjectParameter;
};