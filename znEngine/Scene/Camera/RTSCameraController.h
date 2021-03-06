#pragma once

#include "CameraControllerBase.h"

class ZN_API CRTSCameraController 
	: public CCameraControllerBase
{
public:
	                                                CRTSCameraController();
	virtual                                         ~CRTSCameraController();

	//
	void                                            SetCamera(const std::shared_ptr<ICameraComponent3D>& Camera) override;


	// RTS camera
	void                                            SetBounds(BoundingBox Bounds);
	void                                            SetCameraPosition(glm::vec2 Position);
	void                                            SetCameraDistance(float Distance);

	// Engine events
	void                                            OnUpdate(UpdateEventArgs& e) override;

    // Mouse events
	void                                            OnMouseMoved(MouseMotionEventArgs& e) override;
	void                                            OnMouseWheel(MouseWheelEventArgs& e) override;

private:
	void                                            AddX(float Value);
	void                                            AddY(float Value);
	void                                            AddZ(float Value);

private:
	BoundingBox                                     m_Bounds;
    glm::vec3                                       m_CameraPosition;

	// Multipliers
	float                                           m_MouseMoveMultiplier;
	float                                           m_MouseWheelMultiplier;
};