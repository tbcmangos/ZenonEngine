#pragma once

#include "CameraControllerBase.h"

class ZN_API CArcBallCameraController 
	: public CCameraControllerBase
{
public:
	                                                CArcBallCameraController();
	virtual                                         ~CArcBallCameraController();

	// Pivot
	void                                            SetPivotDistance(float pivotDistance);
	float                                           GetPivotDistance() const;
	glm::vec3                                       GetPivotPoint() const;

	// Engine events
	void                                            OnUpdate(UpdateEventArgs& e);

    // Keyboard events
	void                                            OnKeyPressed(KeyEventArgs& e);
	void                                            OnKeyReleased(KeyEventArgs& e);

    // Mouse events
	void                                            OnMouseButtonPressed(MouseButtonEventArgs& e);
	void                                            OnMouseButtonReleased(MouseButtonEventArgs& e);
	void                                            OnMouseMoved(MouseMotionEventArgs& e);

protected:
	glm::vec3                                       ProjectOntoUnitSphere(glm::ivec2 screenPos);

private:
	// Translation movement
	float Forward, Back, Left, Right, Up, Down;

	// Rotation movement
	float RollCW, RollCCW;
	float Pitch, Yaw;

	// Move in/out from pivot point.
	float PivotTranslate;

	// Used for arcball camera
	glm::vec3   m_PreviousPoint;
	float       m_PivotDistance;

	// Do you want to go faster?
	bool TranslateFaster;
	bool RotateFaster;
};