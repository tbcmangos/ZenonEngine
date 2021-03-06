#include "stdafx.h"

// General
#include "ColliderComponent3D.h"

CColliderComponent3D::CColliderComponent3D(const ISceneNode3D& OwnerNode)
	: CComponentBase(OwnerNode)
	, m_CullStrategy(ECullStrategy::None)
	, m_CullDistance(99999.0f) // Don't use FloatMax
	, m_DebugDraw(true)
{
	GetProperties()->SetName("ColliderComponent");

	{
		auto minBounds = std::make_shared<CPropertyWrapped<glm::vec3>>("BBoxMin", "");
		minBounds->SetValueSetter(std::bind(&CColliderComponent3D::SetMinBounds, this, std::placeholders::_1));
		minBounds->SetValueGetter(std::bind(&CColliderComponent3D::GetMinBounds, this));
		GetProperties()->AddProperty(minBounds);

		auto maxBounds = std::make_shared<CPropertyWrapped<glm::vec3>>("BBoxMax", "");
		maxBounds->SetValueSetter(std::bind(&CColliderComponent3D::SetMaxBounds, this, std::placeholders::_1));
		maxBounds->SetValueGetter(std::bind(&CColliderComponent3D::GetMaxBounds, this));
		GetProperties()->AddProperty(maxBounds);
	}

	// Actions
	{
		std::shared_ptr<CAction> clear = std::make_shared<CAction>("Clear", "Clean BBox. (set to infinite).");
		clear->SetAction([this] () -> bool {
			SetBounds(BoundingBox());
			return true;
		});

		GetProperties()->AddProperty(clear);
	}
}

CColliderComponent3D::~CColliderComponent3D()
{
}

void CColliderComponent3D::Copy(std::shared_ptr<ISceneNodeComponent> Destination) const
{
	auto destCast = std::static_pointer_cast<CColliderComponent3D>(Destination);

	destCast->m_CullStrategy = m_CullStrategy;
	destCast->m_CullDistance = m_CullDistance;
	destCast->m_Bounds = m_Bounds;
	destCast->m_WorldBounds = m_WorldBounds;
	destCast->m_DebugDraw = m_DebugDraw;
}


//
// IColliderComponent3D
//
void CColliderComponent3D::SetCullStrategy(ECullStrategy CullStrategy)
{
	m_CullStrategy = CullStrategy;
}

IColliderComponent3D::ECullStrategy CColliderComponent3D::GetCullStrategy() const
{
	return m_CullStrategy;
}

void CColliderComponent3D::SetCullDistance(float Distance)
{
	m_CullDistance = Distance;
}

float CColliderComponent3D::GetCullDistance() const
{
	return m_CullDistance;
}

void CColliderComponent3D::SetBounds(BoundingBox _bbox)
{
	m_Bounds = _bbox;
	RaiseComponentMessage(UUID_OnBoundsChanget);
}
cbbox CColliderComponent3D::GetBounds() const
{
	if (m_Bounds.IsInfinite())
	{
		if (auto models = GetComponent<IModelsComponent3D>())
		{
			glm::vec3 floatMin(Math::MinFloat);
			glm::vec3 floatMax(Math::MaxFloat);

			BoundingBox newBBox(floatMax, floatMin);
			if (auto model = models->GetModel())
				newBBox.makeUnion(model->GetBounds());

			if (newBBox.getMin() != floatMax && newBBox.getMax() != floatMin)
				const_cast<CColliderComponent3D*>(this)->SetBounds(newBBox);
		}
	}
	return m_Bounds;
}

cbbox CColliderComponent3D::GetWorldBounds() const
{
	return m_WorldBounds;
}

void CColliderComponent3D::SetDebugDrawMode(bool Value)
{
	m_DebugDraw = Value;
}

bool CColliderComponent3D::GetDebugDrawMode() const
{
	return m_DebugDraw;
}

bool CColliderComponent3D::IsCulled(const ICameraComponent3D * Camera) const
{
	switch (m_CullStrategy)
	{
		case ECullStrategy::None:
			return false;
		case ECullStrategy::ByDistance:
			return IsCulledByDistance(Camera);
		case ECullStrategy::ByDistance2D:
			return IsCulledByDistance2D(Camera);
		case ECullStrategy::ByFrustrum:
			return IsCulledByFrustum(Camera);
		case ECullStrategy::ByFrustrumAndDistance:
			return IsCulledByDistance(Camera) || IsCulledByFrustum(Camera);
		case ECullStrategy::ByFrustrumAndDistance2D:
			return IsCulledByDistance2D(Camera) || IsCulledByFrustum(Camera);
		default:
			_ASSERT(false);
	}

	_ASSERT(false);
	return false;
}

bool CColliderComponent3D::IsCulledByFrustum(const ICameraComponent3D* Camera) const
{
	_ASSERT(Camera != nullptr);

	return Camera->GetFrustum().cullBox(GetWorldBounds());
}

bool CColliderComponent3D::IsCulledByDistance2D(const ICameraComponent3D* Camera) const
{
	_ASSERT(Camera != nullptr);

	const glm::vec3 cameraPosition = Camera->GetTranslation();
	const float distToCamera2D = glm::length(Fix_X0Z(cameraPosition) - Fix_X0Z(GetWorldBounds().getCenter())) - GetWorldBounds().getRadius();
	return distToCamera2D > m_CullDistance;
}

bool CColliderComponent3D::IsCulledByDistance(const ICameraComponent3D* Camera) const
{
	_ASSERT(Camera != nullptr);

	const glm::vec3 cameraPosition = Camera->GetTranslation();
	const float distToCamera = glm::length(cameraPosition - GetWorldBounds().getCenter()) - GetWorldBounds().getRadius();
	return distToCamera > m_CullDistance;
}

bool CColliderComponent3D::IsRayIntersects(const Ray & Ray) const
{
	if (GetBounds().IsInfinite())
		return false;

	return HitBoundingBox(GetWorldBounds().getMin(), GetWorldBounds().getMax(), Ray.GetOrigin(), Ray.GetDirection());
}

void CColliderComponent3D::OnMessage(const ISceneNodeComponent* Component, ComponentMessageType Message)
{
	switch (Message)
	{
		case UUID_OnWorldTransformChanged:
		{
			UpdateBounds();
		}
		break;
		case UUID_OnBoundsChanget:
		{
			_ASSERT(Component == this);
			UpdateBounds();
		}
		break;
	}

}



//
// Protected
//
void CColliderComponent3D::UpdateBounds()
{
	BoundingBox bounds = m_Bounds;
	bounds.transform(GetOwnerNode().GetWorldTransfom());
	m_WorldBounds = bounds;
}

void CColliderComponent3D::SetMinBounds(const glm::vec3 & Min)
{
	m_Bounds.setMin(Min);
}

glm::vec3 CColliderComponent3D::GetMinBounds() const
{
	return m_Bounds.getMin();
}

void CColliderComponent3D::SetMaxBounds(const glm::vec3 & Max)
{
	m_Bounds.setMax(Max);
}

glm::vec3 CColliderComponent3D::GetMaxBounds() const
{
	return m_Bounds.getMax();
}
