#include "stdafx.h"

// General
#include "BoundingBox.h"

BoundingBox::BoundingBox(EBBoxMode BBoxMode)
{
	clear(BBoxMode);
}

BoundingBox::BoundingBox(const glm::vec3& Min, const glm::vec3& Max)
{
	set(Min, Max);
}

void BoundingBox::set(const glm::vec3& Min, const glm::vec3& Max)
{
	m_Min = Min;
	m_Max = Max;

	// Fix bounding box
	/*if (_needConvert)
	{
		m_Min = Fix_XZmY(m_Min);
		m_Max = Fix_XZmY(m_Max);
		std::swap(m_Min.z, m_Max.z);
	}*/

	//_ASSERT(min.x < max.x && min.y < max.y && min.z < max.z);

	calculateCenter();
}

void BoundingBox::calculate(const glm::vec3* _verts, uint32 _count)
{
	for (uint32 i = 0; i < _count; i++)
	{
		const glm::vec3 v =/*(_needConvert) ? (Fix_XZmY(_verts[i])) :*/ _verts[i];

		if (v.x < m_Min.x) m_Min.x = v.x;
		if (v.y < m_Min.y) m_Min.y = v.y;
		if (v.z < m_Min.z) m_Min.z = v.z;

		if (v.x > m_Max.x) m_Max.x = v.x;
		if (v.y > m_Max.y) m_Max.y = v.y;
		if (v.z > m_Max.z) m_Max.z = v.z;
	}

	calculateCenter();
}

void BoundingBox::calculateCenter()
{
	m_Center = (m_Min + m_Max) * 0.5f;
	m_Radius = glm::length(m_Max - m_Center);
	m_IsCenterCalc = true;
}

void BoundingBox::clear(EBBoxMode BBoxMode)
{
	if (BBoxMode == BBoxMode_Infinite)
	{
		m_Min = glm::vec3(Math::MinFloat);
		m_Max = glm::vec3(Math::MaxFloat);
	}
	else if (BBoxMode == BBoxMode_Incorrect)
	{
		m_Min = glm::vec3(Math::MaxFloat);
		m_Max = glm::vec3(Math::MinFloat);
	}
	else
		_ASSERT(false);

	m_Center = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Radius = Math::MaxFloat / 2.0f;
	m_IsCenterCalc = true;
}

bool BoundingBox::IsInfinite() const
{
	return (m_Min.x == Math::MinFloat) || (m_Min.y == Math::MinFloat) || (m_Min.z == Math::MinFloat) || 
		   (m_Max.x == Math::MaxFloat) || (m_Max.y == Math::MaxFloat) || (m_Max.z == Math::MaxFloat);
}

bool BoundingBox::IsIncorrect() const
{
	return (m_Min.x == Math::MaxFloat) || (m_Min.y == Math::MaxFloat) || (m_Min.z == Math::MaxFloat) ||
		   (m_Max.x == Math::MinFloat) || (m_Max.y == Math::MinFloat) || (m_Max.z == Math::MinFloat);
}

//

glm::vec3 BoundingBox::getCorner(uint32 index) const
{
	switch (index)
	{
	case 0:
		return glm::vec3(m_Min.x, m_Min.y, m_Max.z);
	case 1:
		return glm::vec3(m_Max.x, m_Min.y, m_Max.z);
	case 2:
		return glm::vec3(m_Max.x, m_Max.y, m_Max.z);
	case 3:
		return glm::vec3(m_Min.x, m_Max.y, m_Max.z);
	case 4:
		return glm::vec3(m_Min.x, m_Min.y, m_Min.z);
	case 5:
		return glm::vec3(m_Max.x, m_Min.y, m_Min.z);
	case 6:
		return glm::vec3(m_Max.x, m_Max.y, m_Min.z);
	case 7:
		return glm::vec3(m_Min.x, m_Max.y, m_Min.z);
	default:
		return glm::vec3();
	}
}

void BoundingBox::transform(const glm::mat4& m)
{
	// Efficient algorithm for transforming an AABB, taken from Graphics Gems

	float minA[3] = { m_Min.x, m_Min.y, m_Min.z };
	float minB[3];
	float maxA[3] = { m_Max.x, m_Max.y, m_Max.z };
	float maxB[3];

	for (uint32 i = 0; i < 3; ++i)
	{
		minB[i] = m[3][i];
		maxB[i] = m[3][i];

		for (uint32 j = 0; j < 3; ++j)
		{
			float x = minA[j] * m[j][i];
			float y = maxA[j] * m[j][i];
			minB[i] += minf(x, y);
			maxB[i] += maxf(x, y);
		}
	}

	m_Min = glm::vec3(minB[0], minB[1], minB[2]);
	m_Max = glm::vec3(maxB[0], maxB[1], maxB[2]);

	calculateCenter();
}

bool BoundingBox::makeUnion(const BoundingBox& b)
{
	bool changed = false;

	// Ignore zero-size boxes
	if (m_Min == m_Max)
	{
		changed = true;
		m_Min = b.m_Min;
		m_Max = b.m_Max;
	}
	else if (b.m_Min != b.m_Max)
	{
		if (b.m_Min.x < m_Min.x) { changed = true; m_Min.x = b.m_Min.x; }
		if (b.m_Min.y < m_Min.y) { changed = true; m_Min.y = b.m_Min.y; }
		if (b.m_Min.z < m_Min.z) { changed = true; m_Min.z = b.m_Min.z; }

		if (b.m_Max.x > m_Max.x) { changed = true; m_Max.x = b.m_Max.x; }
		if (b.m_Max.y > m_Max.y) { changed = true; m_Max.y = b.m_Max.y; }
		if (b.m_Max.z > m_Max.z) { changed = true; m_Max.z = b.m_Max.z; }
	}

	calculateCenter();

	return changed;
}

bool BoundingBox::isPointInside(const glm::vec3& _point) const
{
	if (_point.x < m_Min.x || _point.y < m_Min.y ||	_point.z < m_Min.z ||
		_point.x > m_Max.x || _point.y > m_Max.y ||	_point.z > m_Max.z)
	{
		return false;
	}

	return true;
}

