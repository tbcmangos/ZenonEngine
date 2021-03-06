#include "stdafx.h"

// General
#include "AnimatorComponent.h"

CAnimatorComponent3D::CAnimatorComponent3D(const ISceneNode3D & Owner)
	: CComponentBase(Owner)
	, m_IsLoop(false)
	, m_IsStopped(false)
	, animtime(0.0)
	, m_CurrentAnimation(nullptr)
	, m_CurrentAnimationIndex(0)
	, m_CurrentTime(0)
{
}

CAnimatorComponent3D::~CAnimatorComponent3D()
{
}

void CAnimatorComponent3D::AddAnimation(uint16 AnimationId, const SAnimation & Animation)
{
	m_Animations.insert(std::make_pair(AnimationId, Animation));

	if (m_CurrentAnimation == nullptr)
		PlayAnimation(AnimationId, true);
}

void CAnimatorComponent3D::PlayAnimation(uint16 AnimationId, bool Loop)
{
	m_IsLoop = Loop;

	const auto& animIt = m_Animations.find(AnimationId);
	if (animIt != m_Animations.end())
	{
		m_CurrentAnimation = &animIt->second;
		m_CurrentAnimationIndex = animIt->first;
	}
	else
	{
		_ASSERT(m_Animations.size() > 0);
		m_CurrentAnimation = &m_Animations.begin()->second;
		m_CurrentAnimationIndex = 0;
		//Log::Error("CM2_Animator: Animation '%d' not found. Playing first animation '%s' ('%d').", AnimationId, m_CurrentAnimation->getAnimationName().c_str(), m_CurrentAnimation->getAnimID());
	}

	m_CurrentTime = m_CurrentAnimation->FrameStart;
	m_IsStopped = false;
	animtime = 0.0;
}

uint16 CAnimatorComponent3D::getSequenceIndex() const
{
	return m_CurrentAnimationIndex;
}

uint32 CAnimatorComponent3D::getCurrentTime() const
{
	return m_CurrentTime;
}

void CAnimatorComponent3D::Update(const UpdateEventArgs & e)
{
	if (m_IsStopped)
		return;

	// 37 ������

	float fpsMultiplier = (e.DeltaTime) / 1000.0f * 60.0;

	animtime += fpsMultiplier;
	m_CurrentTime = static_cast<uint32>(m_CurrentAnimation->FrameStart + animtime);

	// Animation don't ended
	if (m_CurrentTime < m_CurrentAnimation->FrameEnd)
		return;

	// Ended!
	/*if (m_CurrentAnimation->hasNextVatianton())
	{
		m_CurrentAnimation = m_CurrentAnimation->getNextVariation();
		m_CurrentTime = m_CurrentAnimation->getStart();
		m_IsPlayed = false;
		animtime = 0;
		return;
	}*/

	m_CurrentTime = m_CurrentAnimation->FrameEnd - 1;
	m_IsStopped = true;

	if (m_IsLoop)
	{
		PlayAnimation(m_CurrentAnimationIndex, true);
	}
}
