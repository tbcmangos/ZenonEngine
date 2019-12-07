#pragma once

// FORWARD BEGIN
ZN_INTERFACE IScene;
// FORWARD END

struct OW_ENGINE_API IGameState
{
	virtual ~IGameState() = 0 {};

	virtual bool Init() = 0;
	virtual void Destroy() = 0;
	virtual bool Set() = 0;
	virtual void Unset() = 0;

	virtual void SetInited(bool _value) = 0;
	virtual bool IsInited() const = 0;
	virtual void SetCurrent(bool _value) = 0;
	virtual bool IsCurrent() const = 0;

	// Keyboard events
	virtual      void OnKeyPressed(KeyEventArgs& e) = 0;
	virtual      void OnKeyReleased(KeyEventArgs& e) = 0;
	virtual      void OnKeyboardFocus(EventArgs& e) = 0;
	virtual      void OnKeyboardBlur(EventArgs& e) = 0;

	// Mouse events
	virtual      void OnMouseMoved(MouseMotionEventArgs& e) = 0;
	virtual      void OnMouseButtonPressed(MouseButtonEventArgs& e) = 0;
	virtual      void OnMouseButtonReleased(MouseButtonEventArgs& e) = 0;
	virtual      void OnMouseWheel(MouseWheelEventArgs& e) = 0;
	virtual      void OnMouseLeave(EventArgs& e) = 0;
	virtual      void OnMouseFocus(EventArgs& e) = 0;
	virtual      void OnMouseBlur(EventArgs& e) = 0;

	virtual std::shared_ptr<IScene> GetScene3D() const = 0;
};