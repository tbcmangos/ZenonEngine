#pragma once

class CMaterialPassTransperent
	: public Base3DPass
{
public:
	CMaterialPassTransperent(IRenderDevice* RenderDevice, std::shared_ptr<IScene> Scene);
	virtual ~CMaterialPassTransperent();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> CreatePipeline(IRenderTarget* RenderTarget, const Viewport* Viewport) override;

	// IVisitor
	bool Visit(IMesh* Mesh, SGeometryPartParams GeometryPartParams = SGeometryPartParams()) override;
	bool Visit(IGeometry* Geometry, const IMaterial* Material, SGeometryPartParams GeometryPartParams = SGeometryPartParams()) override;
};