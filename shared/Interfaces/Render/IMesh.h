#pragma once

#include "Interfaces/Render/Types/RenderGeometryParams.h"

// FORWARD BEGIN
ZN_INTERFACE IBuffer;
ZN_INTERFACE IGeometry;
ZN_INTERFACE IConstantBuffer;
class RenderEventArgs;
// FORWARD END

ZN_INTERFACE ZN_API __declspec(novtable) IMesh : public std::enable_shared_from_this<IMesh>
{
	virtual ~IMesh() {}

	virtual void                                    SetName(const std::string& Name) = 0;
	virtual std::string                             GetName() const = 0;

	virtual void                                    SetBounds(const BoundingBox& Bounds) = 0;
	virtual const BoundingBox&                      GetBounds() const = 0;

	virtual void                                    AddVertexBuffer(const BufferBinding& binding, IBuffer* buffer) = 0;
	virtual void                                    SetVertexBuffer(IBuffer* buffer) = 0;
	virtual void                                    SetIndexBuffer(IBuffer* buffer) = 0;

	virtual void                                    SetPrimitiveTopology(PrimitiveTopology _topology) = 0;

	virtual void                                    SetMaterial(const IMaterial* Material) = 0;
	virtual void                                    AddMaterial(const IMaterial* Material, SGeometryPartParams GeometryPartParams = SGeometryPartParams()) = 0;

	virtual IGeometry*                              GetGeometry() const = 0;

	virtual bool                                    Render(const RenderEventArgs* renderEventArgs, const IConstantBuffer* perObject, SGeometryPartParams GeometryPartParams = SGeometryPartParams()) = 0;

	virtual bool                                    Accept(IVisitor* visitor, SGeometryPartParams GeometryPartParams = SGeometryPartParams()) = 0;
};

typedef std::vector<IMesh*> MeshList;
