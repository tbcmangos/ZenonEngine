#include "stdafx.h"

// General
#include "ModelProxie.h"

ModelProxie::ModelProxie(const std::shared_ptr<IModel>& Model)
	: m_Model(Model)
{
	_ASSERT(m_Model != nullptr);
}

ModelProxie::~ModelProxie()
{}



//
// IModel
//
void ModelProxie::SetBounds(const BoundingBox & Bounds)
{
	m_Model->SetBounds(Bounds);
}

BoundingBox ModelProxie::GetBounds() const
{
	return m_Model->GetBounds();
}

void ModelProxie::SetFileName(const std::string & FileName)
{
	m_Model->SetFileName(FileName);
}

std::string ModelProxie::GetFileName() const
{
	return m_Model->GetFileName();
}

void ModelProxie::AddConnection(const std::shared_ptr<IMaterial>& Material, const std::shared_ptr<IGeometry>& Geometry, SGeometryDrawArgs GeometryDrawArgs)
{
	m_Model->AddConnection(Material, Geometry, GeometryDrawArgs);
}

const std::vector<ModelProxie::SConnection>& ModelProxie::GetConnections() const
{
	return m_Model->GetConnections();
}

bool ModelProxie::Render(const RenderEventArgs& renderEventArgs) const
{
	return m_Model->Render(renderEventArgs);
}

void ModelProxie::Accept(IVisitor* visitor)
{
	EVisitResult visitResult = visitor->Visit(this);

	if (visitResult & EVisitResult::AllowVisitContent)
	{

	}

	if (visitResult & EVisitResult::AllowVisitChilds)
	{
		for (const auto& connection : GetConnections())
		{
			connection.Geometry->Accept(visitor, connection.Material.get(), connection.GeometryDrawArgs);
		}
	}
}



//
// IObjectLoadSave
//
void ModelProxie::Load(const std::shared_ptr<IByteBuffer>& ByteBuffer)
{
	if (const auto& loadableFromFile = std::dynamic_pointer_cast<IObjectLoadSave>(m_Model))
	{
		loadableFromFile->Load(ByteBuffer);
	}
	else
		_ASSERT(false);
}

void ModelProxie::Save(const std::shared_ptr<IByteBuffer>& ByteBuffer) const
{
	if (const auto& loadableFromFile = std::dynamic_pointer_cast<IObjectLoadSave>(m_Model))
	{
		loadableFromFile->Save(ByteBuffer);
	}
	else
		_ASSERT(false);
}

void ModelProxie::Load(const std::shared_ptr<IXMLReader>& Reader)
{
	_ASSERT(false);
}

void ModelProxie::Save(const std::shared_ptr<IXMLWriter>& Writer) const
{
	_ASSERT(false);
}

const std::shared_ptr<IModel> ModelProxie::GetModel() const
{
	return m_Model;
}
