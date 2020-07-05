#pragma once

#include "3D/SceneNode3D.h"
#include "UI/SceneNodeUI.h"
#include "../Passes/RenderTechnique.h"

class ZN_API SceneBase 
	: public IScene
	, public INativeWindowEventListener
	, public IBaseManagerHolder
	, public Object
{
public:
	SceneBase(IBaseManager& BaseManager);
	virtual ~SceneBase();

	// IScene
	void                                            SetRenderWindow(const std::shared_ptr<IRenderWindow>& RenderWindow) override;
	void                                            ConnectEvents(const std::shared_ptr<IRenderWindowEvents>& WindowEvents) override;
	void                                            DisconnectEvents(const std::shared_ptr<IRenderWindowEvents>& WindowEvents) override;

	virtual void                                    Initialize();
	virtual void                                    Finalize();

	std::shared_ptr<ISceneNode3D>					GetRootNode3D() const override;
	std::shared_ptr<ISceneNodeUI>					GetRootNodeUI() const override;
	void                                            SetCameraController(std::shared_ptr<ICameraController> CameraController);
	std::shared_ptr<ICameraController>              GetCameraController() const;

	// Visit funcitonal
	void                                            Accept(IVisitor* visitor) override;
	void											Freeze() override;
	void											Unfreeze() override;
	void                                            AddChild(const std::shared_ptr<ISceneNode3D>& ParentNode, const std::shared_ptr<ISceneNode3D>& ChildNode) override;
	void                                            RemoveChild(const std::shared_ptr<ISceneNode3D>& ParentNode, const std::shared_ptr<ISceneNode3D>& ChildNode) override;

	// Scene events
	Delegate<SceneChangeEventArgs>&					SceneChangeEvent() override;
	void                                            RaiseSceneChangeEvent(ESceneChangeType SceneChangeType, const std::shared_ptr<ISceneNode3D>& OwnerNode, const std::shared_ptr<ISceneNode3D>& ChildNode) override;
	virtual bool                                    OnMouseClickToWorld(MouseButtonEventArgs::MouseButton& MouseButton, const glm::vec2& MousePosition, const Ray& RayToWorld);
	virtual void                                    OnMouseReleaseToWorld(MouseButtonEventArgs::MouseButton& MouseButton, const glm::vec2& MousePosition, const Ray& RayToWorld);
	virtual void                                    OnMouseMoveToWorld(MouseButtonEventArgs::MouseButton& MouseButton, const glm::vec2& MousePosition, const Ray& RayToWorld);

	// Engine events
	virtual void                                    OnUpdate(UpdateEventArgs& e) ;
	virtual void                                    OnPreRender(RenderEventArgs& e);
	virtual void                                    OnRender(RenderEventArgs& e);
	virtual void                                    OnPostRender(RenderEventArgs& e);
	virtual void                                    OnRenderUI(RenderEventArgs& e);

	// INativeWindowEventListener
	// Window events
	virtual void                                    OnWindowInputFocus(EventArgs& Args) override {} // Window gets input focus
	virtual void                                    OnWindowInputBlur(EventArgs& Args) override {}  // Window loses input focus
	virtual void                                    OnWindowMinimize(EventArgs& Args) override {}   // Window is minimized.
	virtual void                                    OnWindowRestore(EventArgs& Args) override {}    // Window is restored.
	virtual void                                    OnWindowResize(ResizeEventArgs& Args) override;
	virtual void                                    OnWindowClose(WindowCloseEventArgs& Args) override {}

	// Keyboard events
	virtual bool                                    OnWindowKeyPressed(KeyEventArgs& e) override;
	virtual void                                    OnWindowKeyReleased(KeyEventArgs& e) override;
	virtual void                                    OnWindowKeyboardFocus(EventArgs& Args) override {}
	virtual void                                    OnWindowKeyboardBlur(EventArgs& Args) override {}

	// Mouse events
	virtual void                                    OnWindowMouseMoved(MouseMotionEventArgs& e) override;
	virtual bool                                    OnWindowMouseButtonPressed(MouseButtonEventArgs& e) override;
	virtual void                                    OnWindowMouseButtonReleased(MouseButtonEventArgs& e) override;
	virtual bool                                    OnWindowMouseWheel(MouseWheelEventArgs& e) override;
	virtual void                                    OnWindowMouseLeave(EventArgs& e) override {}
	virtual void                                    OnWindowMouseFocus(EventArgs& e) override {}
	virtual void                                    OnWindowMouseBlur(EventArgs& e) override {}

	// IBaseManagerHolder
	IBaseManager&                                   GetBaseManager() const override final;


protected:
	IRenderDevice&                                  GetRenderDevice() const;
	std::shared_ptr<IRenderWindow>                  GetRenderWindow() const;

	std::map<float, std::shared_ptr<ISceneNode3D>>  FindIntersection(const Ray& Ray) const;
	std::vector<std::shared_ptr<ISceneNode3D>>      FindIntersections(const Frustum& Frustum) const;

protected: // Input events process recursive
	void                                            DoUpdate_Rec(const std::shared_ptr<ISceneNode3D>& Node, const UpdateEventArgs& e);

	bool                                            DoKeyPressed_Rec(const std::shared_ptr<ISceneNodeUI>& Node, KeyEventArgs& e);
	void                                            DoKeyReleased_Rec(const std::shared_ptr<ISceneNodeUI>& Node, KeyEventArgs& e);
	void                                            DoMouseMoved_Rec(const std::shared_ptr<ISceneNodeUI>& Node, MouseMotionEventArgs& e);
	bool                                            DoMouseButtonPressed_Rec(const std::shared_ptr<ISceneNodeUI>& Node, MouseButtonEventArgs& e);
	void                                            DoMouseButtonReleased_Rec(const std::shared_ptr<ISceneNodeUI>& Node, MouseButtonEventArgs& e);
	bool                                            DoMouseWheel_Rec(const std::shared_ptr<ISceneNodeUI>& Node, MouseWheelEventArgs& e);

protected:
	RenderTechnique                                 m_Technique3D;
	RenderTechnique                                 m_TechniqueUI;

	std::shared_ptr<ISceneNode3D>                   m_RootNode3D;
	std::shared_ptr<ISceneNodeUI>                   m_RootNodeUI;

	std::shared_ptr<IQuery>                         m_FrameQuery;
	std::shared_ptr<IQuery>                         m_TestQuery;
	double                                          m_FrameTime;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_Start, m_End;

	std::shared_ptr<ICameraController>              m_DefaultCameraController;
	std::shared_ptr<ISettingGroup>                  m_VideoSettings;

	std::shared_ptr<ISceneNodeUI>                   m_CameraPosText;
	std::shared_ptr<ISceneNodeUI>                   m_CameraRotText;
	std::shared_ptr<ISceneNodeUI>                   m_CameraRot2Text;
	std::shared_ptr<ISceneNodeUI>                   m_FPSText;


private: // IRenderWindowEvents
	Delegate<UpdateEventArgs>::FunctionDecl         m_OnUpdateConnection;
	Delegate<RenderEventArgs>::FunctionDecl         m_OnPreRenderConnection;
	Delegate<RenderEventArgs>::FunctionDecl         m_OnRenderConnection;
	Delegate<RenderEventArgs>::FunctionDecl         m_OnPostRenderConnection;
	Delegate<RenderEventArgs>::FunctionDecl         m_OnRenderUIConnection;

private: // Windows events connection
	Delegate<ResizeEventArgs>::FunctionDecl         m_OnResizeConnection;

private: // Keyboard events connections
	Delegate<KeyEventArgs>::FunctionDecl            m_OnKeyPressedConnection;
	Delegate<KeyEventArgs>::FunctionDecl            m_OnKeyReleasedConnection;

private: // Mouse events connections
	Delegate<MouseButtonEventArgs>::FunctionDecl    m_OnMouseButtonPressedConnection;
	Delegate<MouseButtonEventArgs>::FunctionDecl    m_OnMouseButtonReleasedConnection;
	Delegate<MouseMotionEventArgs>::FunctionDecl    m_OnMouseMovedConnection;
	Delegate<MouseWheelEventArgs>::FunctionDecl     m_OnMouseWheelConnection;

private:
	Delegate<SceneChangeEventArgs>					m_SceneChangeEvent;

protected: // ���������� �� ����������� ���������� ���
	std::vector<std::pair<std::shared_ptr<ISceneNode3D>, std::shared_ptr<ISceneNode3D>>> m_AddChildList;
	std::vector<std::pair<std::shared_ptr<ISceneNode3D>, std::shared_ptr<ISceneNode3D>>> m_RemoveChildList;
	std::mutex                                                                           m_ListsAreBusy;
	std::mutex                                                                           m_SceneIsBusy;

private: // Quick access
	IBaseManager&                                   m_BaseManager;
	IRenderDevice&                                  m_RenderDevice;
	std::weak_ptr<IRenderWindow>                    m_RenderWindow;
	std::mutex                                      m_SceneMutex;
	std::atomic_bool                                m_IsFreezed;
};
