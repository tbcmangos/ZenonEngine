#include "stdafx.h"

// General
#include "RenderDeviceBase.h"

// Additional
#include "Primitives/Geometry.h"

CRenderDevicePrimitiveCollection::CRenderDevicePrimitiveCollection(IRenderDevice * RenderDevice)
	: m_RenderDevice(RenderDevice)
{
}

CRenderDevicePrimitiveCollection::~CRenderDevicePrimitiveCollection()
{
}

std::shared_ptr<IMesh> CRenderDevicePrimitiveCollection::CreateLine(cvec3 _dest)
{
	vec3 p[2];
	p[0] = vec3(0.0f, 0.0f, 0.0f);
	p[1] = _dest;

	std::shared_ptr<IMesh> mesh = m_RenderDevice->CreateMesh();
	mesh->SetPrimitiveTopology(PrimitiveTopology::LineList);

	std::shared_ptr<IBuffer> __vb = m_RenderDevice->CreateVertexBuffer(p, 2);
	mesh->AddVertexBuffer(BufferBinding("POSITION", 0), __vb);

	return mesh;
}

std::shared_ptr<IMesh> CRenderDevicePrimitiveCollection::CreatePlane(cvec3 N)
{
	DirectX::VertexCollection vertices;
	vertices.push_back(DirectX::VertexPositionTextureNormal(DirectX::XMFLOAT3( 1.0f, 0.0f,  1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)));
	vertices.push_back(DirectX::VertexPositionTextureNormal(DirectX::XMFLOAT3(-1.0f, 0.0f,  1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)));
	vertices.push_back(DirectX::VertexPositionTextureNormal(DirectX::XMFLOAT3(-1.0f, 0.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)));
	vertices.push_back(DirectX::VertexPositionTextureNormal(DirectX::XMFLOAT3(1.0f, 0.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)));

	DirectX::IndexCollection indices;
	indices.push_back(1);
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(3);

	std::shared_ptr<IMesh> mesh = m_RenderDevice->CreateMesh();

	std::shared_ptr<IBuffer> __vb = m_RenderDevice->CreateVoidVertexBuffer(vertices.data(), vertices.size(), 0, sizeof(DirectX::VertexPositionTextureNormal));
	mesh->SetVertexBuffer(__vb);

	std::shared_ptr<IBuffer> __ib = m_RenderDevice->CreateIndexBuffer(indices);
	mesh->SetIndexBuffer(__ib);

	return mesh;
}

std::shared_ptr<IMesh> CRenderDevicePrimitiveCollection::CreateScreenQuad(float left, float right, float bottom, float top, float z)
{
	DirectX::VertexCollection vertices;
	vertices.push_back(DirectX::VertexPositionTextureNormal(DirectX::XMFLOAT3(right, top, z), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f)));
	vertices.push_back(DirectX::VertexPositionTextureNormal(DirectX::XMFLOAT3(left, top, z), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f)));
	vertices.push_back(DirectX::VertexPositionTextureNormal(DirectX::XMFLOAT3(left, bottom, z), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f)));
	vertices.push_back(DirectX::VertexPositionTextureNormal(DirectX::XMFLOAT3(right, bottom, z), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f)));

	DirectX::IndexCollection indices;
	indices.push_back(1);
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(3);

	std::shared_ptr<IMesh> mesh = m_RenderDevice->CreateMesh();

	std::shared_ptr<IBuffer> __vb = m_RenderDevice->CreateVoidVertexBuffer(vertices.data(), vertices.size(), 0, sizeof(DirectX::VertexPositionTextureNormal));
	mesh->SetVertexBuffer(__vb);

	std::shared_ptr<IBuffer> __ib = m_RenderDevice->CreateIndexBuffer(indices);
	mesh->SetIndexBuffer(__ib);

	return mesh;
}

std::shared_ptr<IMesh> CRenderDevicePrimitiveCollection::CreateSphere()
{
	DirectX::VertexCollection vertices;
	DirectX::IndexCollection indices;
	DirectX::ComputeSphere(vertices, indices, 1.0f, 32, false, false);

	std::shared_ptr<IMesh> mesh = m_RenderDevice->CreateMesh();

	std::shared_ptr<IBuffer> __vb = m_RenderDevice->CreateVoidVertexBuffer(vertices.data(), vertices.size(), 0, sizeof(DirectX::VertexPositionTextureNormal));
	mesh->SetVertexBuffer(__vb);

	std::shared_ptr<IBuffer> __ib = m_RenderDevice->CreateIndexBuffer(indices);
	mesh->SetIndexBuffer(__ib);

	return mesh;
}

std::shared_ptr<IMesh> CRenderDevicePrimitiveCollection::CreateCube()
{
	DirectX::VertexCollection vertices;
	DirectX::IndexCollection indices;
	DirectX::ComputeBox(vertices, indices, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), false, false);

	std::shared_ptr<IMesh> mesh = m_RenderDevice->CreateMesh();
	mesh->SetPrimitiveTopology(PrimitiveTopology::TriangleList);

	std::shared_ptr<IBuffer> __vb = m_RenderDevice->CreateVoidVertexBuffer(vertices.data(), vertices.size(), 0, sizeof(DirectX::VertexPositionTextureNormal));
	mesh->SetVertexBuffer(__vb);

	std::shared_ptr<IBuffer> __ib = m_RenderDevice->CreateIndexBuffer(indices);
	mesh->SetIndexBuffer(__ib);

	return mesh;
}


std::shared_ptr<IMesh> CRenderDevicePrimitiveCollection::CreateCone()
{
	DirectX::VertexCollection vertices;
	DirectX::IndexCollection indices;
	DirectX::ComputeCone(vertices, indices, 0.77f, 1.0f, 32, false);

	std::shared_ptr<IMesh> mesh = m_RenderDevice->CreateMesh();
	mesh->SetPrimitiveTopology(PrimitiveTopology::TriangleList);

	std::shared_ptr<IBuffer> __vb = m_RenderDevice->CreateVoidVertexBuffer(vertices.data(), vertices.size(), 0, sizeof(DirectX::VertexPositionTextureNormal));
	mesh->SetVertexBuffer(__vb);

	std::shared_ptr<IBuffer> __ib = m_RenderDevice->CreateIndexBuffer(indices);
	mesh->SetIndexBuffer(__ib);

	return mesh;
}



//
// 3D meshes
//
std::shared_ptr<IMesh> CRenderDevicePrimitiveCollection::Create3DQuad(float width, float height)
{
    vec3 vertices[4];
    vertices[0] = vec3(width, 0.0f, height);
    vertices[1] = vec3(0.0f, 0.0f, height);
    vertices[2] = vec3(0.0f, 0.0f, 0.0f);
    vertices[3] = vec3(width, 0.0f, 0.0f);

    vec2 texCoords[4];
    texCoords[0] = vec2(1.0f, 0.0f);
    texCoords[1] = vec2(0.0f, 0.0f);
    texCoords[2] = vec2(0.0f, 1.0f);
    texCoords[3] = vec2(1.0f, 1.0f);

    uint16 indices[6];
    indices[0] = 1;
    indices[1] = 0;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 0;
    indices[5] = 3;

    std::shared_ptr<IMesh> mesh = m_RenderDevice->CreateMesh();

    std::shared_ptr<IBuffer> __vbPosition = m_RenderDevice->CreateVertexBuffer(vertices, 4);
    mesh->AddVertexBuffer(BufferBinding("POSITION", 0), __vbPosition);

    std::shared_ptr<IBuffer> __vbTexCoord = m_RenderDevice->CreateVertexBuffer(texCoords, 4);
    mesh->AddVertexBuffer(BufferBinding("TEXCOORD", 0), __vbTexCoord);

    std::shared_ptr<IBuffer> __ib = m_RenderDevice->CreateIndexBuffer(indices, 6);
    mesh->SetIndexBuffer(__ib);

    return mesh;
}

std::shared_ptr<IMesh> CRenderDevicePrimitiveCollection::Create3DBeizerLine(vec3 start, vec3 end)
{
    vec3 p[4];
    p[0] = start;
    p[1] = vec3((end - start).x / 2.0f, start.y, 0.0f);
    p[2] = vec3((end - start).x / 2.0f, end.y, 0.0f);
    p[3] = end;

    std::shared_ptr<IMesh> mesh = m_RenderDevice->CreateMesh();
    mesh->SetPrimitiveTopology(PrimitiveTopology::LineList);

    std::shared_ptr<IBuffer> __vbPosition = m_RenderDevice->CreateVertexBuffer(p, 4);
    mesh->AddVertexBuffer(BufferBinding("POSITION", 0), __vbPosition);

    return mesh;
}



//
// UI meshes
//
std::shared_ptr<IMesh> CRenderDevicePrimitiveCollection::CreateUIQuad(float width, float height)
{
    vec2 vertices[4];
    vertices[0] = vec2(width, height);
    vertices[1] = vec2(0.0f, height);
    vertices[2] = vec2(0.0f, 0.0f);
    vertices[3] = vec2(width, 0.0f);

    vec2 texCoords[4];
    texCoords[0] = vec2(1.0f, 0.0f);
    texCoords[1] = vec2(0.0f, 0.0f);
    texCoords[2] = vec2(0.0f, 1.0f);
    texCoords[3] = vec2(1.0f, 1.0f);

    uint16 indices[6];
    indices[0] = 1;
    indices[1] = 0;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 0;
    indices[5] = 3;

    std::shared_ptr<IMesh> mesh = m_RenderDevice->CreateMesh();

    std::shared_ptr<IBuffer> __vbPosition = m_RenderDevice->CreateVertexBuffer(vertices, 4);
    mesh->AddVertexBuffer(BufferBinding("POSITION", 0), __vbPosition);

    std::shared_ptr<IBuffer> __vbTexCoord = m_RenderDevice->CreateVertexBuffer(texCoords, 4);
    mesh->AddVertexBuffer(BufferBinding("TEXCOORD", 0), __vbTexCoord);

    std::shared_ptr<IBuffer> __ib = m_RenderDevice->CreateIndexBuffer(indices, 6);
    mesh->SetIndexBuffer(__ib);

    return mesh;
}

std::shared_ptr<IMesh> CRenderDevicePrimitiveCollection::CreateUIBeizerLine(vec2 start, vec2 end)
{
    vec2 vertices[4];
    vertices[0] = start;
    vertices[1] = vec2((end - start).x / 2.0f, start.y);
    vertices[2] = vec2((end - start).x / 2.0f, end.y);
    vertices[3] = end;

    std::shared_ptr<IMesh> mesh = m_RenderDevice->CreateMesh();
    mesh->SetPrimitiveTopology(PrimitiveTopology::LineList);

    std::shared_ptr<IBuffer> __vbPosition = m_RenderDevice->CreateVertexBuffer(vertices, 4);
    mesh->AddVertexBuffer(BufferBinding("POSITION", 0), __vbPosition);

    return mesh;
}

RenderDeviceBase::RenderDeviceBase()
{
	m_RenderDevicePrimitiveCollection = std::make_shared<CRenderDevicePrimitiveCollection>(this);
}

RenderDeviceBase::~RenderDeviceBase()
{
}

std::shared_ptr<IRenderDevicePrimitiveCollection> RenderDeviceBase::GetPrimitiveCollection() const
{
	return m_RenderDevicePrimitiveCollection;
}