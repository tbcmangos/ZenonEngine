#pragma once

class ZN_API SceneNode3D
	: public ISceneNode3D
{
    friend IScene;
public:
	SceneNode3D();
	virtual ~SceneNode3D();

	//
	// ISceneNode3D
	//
	virtual void                                    Initialize() override;
	virtual void                                    Finalize() override;

	// Name & Type
	void                                            SetType(SceneNodeType Type) override;
	SceneNodeType									GetType() const override;
	bool                                            Is(SceneNodeType SceneNodeType) const;
	void                                            SetName(std::string Name) override;
	std::string										GetName() const override;

	// Childs functional
	virtual void                                    AddChild(const std::shared_ptr<ISceneNode3D>& childNode) override;
	virtual void                                    RemoveChild(const std::shared_ptr<ISceneNode3D>& childNode) override;
	virtual std::weak_ptr<ISceneNode3D>             GetParent() const override;
	virtual const Node3DList&                       GetChilds() override;
	void                                            RaiseOnParentChanged() override;

	// Actions & Properties
	IActionsGroup*									GetActions() const override;
	IPropertiesGroup*								GetProperties() const override;
	IScene*											GetScene() const override;

	//
	// Transform functional
	//
	void											SetTranslate(const glm::vec3& Translate) override;
	void                                            AddTranslate(const glm::vec3& Translate) override;
	const glm::vec3&								GetTranslation() const override;
	void											SetRotation(const glm::vec3& _rotate) override;
	const glm::vec3&                                GetRotation() const override;
	void											SetRotationQuaternion(cquat _rotate) override;
	const glm::quat&								GetRotationQuaternion() const override;
	void											SetScale(const glm::vec3& _scale) override;
	const glm::vec3&								GetScale() const override;

	virtual mat4									GetLocalTransform() const;
	virtual mat4									GetInverseLocalTransform() const;
	virtual void									SetLocalTransform(cmat4 localTransform);

	virtual mat4									GetWorldTransfom() const;
	virtual mat4									GetInverseWorldTransform() const;
	virtual mat4									GetParentWorldTransform() const;
	virtual void									SetWorldTransform(cmat4 worldTransform);


	//
	// Components engine
	//
	bool                                            IsComponentExists(GUID ComponentID) const override;
	std::shared_ptr<ISceneNodeComponent>            GetComponent(GUID ComponentID) const override;
	std::shared_ptr<ISceneNodeComponent>            AddComponent(GUID ComponentID, const std::shared_ptr<ISceneNodeComponent>& Component) override;
	const ComponentsMap&                            GetComponents() const override;
	void                                            RaiseComponentMessage(const ISceneNodeComponent* Component, ComponentMessageType Message) const override;
	virtual void                                    RegisterComponents() override;
    
	template<typename T> inline bool                IsComponentExists() const
	{
		return ISceneNode3D::IsComponentExists<T>();
	}
	template<typename T> inline std::shared_ptr<T>  GetComponent() const
	{
		return ISceneNode3D::GetComponent<T>();
	}
	template<typename T> inline std::shared_ptr<T>  AddComponent(const std::shared_ptr<T>& Component)
	{
		return ISceneNode3D::AddComponent<T>(Component);
	}

	const std::shared_ptr<IColliderComponent3D>&    GetColliderComponent() const;
	const std::shared_ptr<IModelsComponent3D>&      GetModelsComponent() const;


	//
	// Others
	//
	virtual void                                    Update(const UpdateEventArgs& e) override;
	virtual void                                    Accept(IVisitor* visitor) override;


private:
	void                                            SetSceneInternal(const std::weak_ptr<IScene>& Scene);
	void                                            AddChildInternal(const std::shared_ptr<ISceneNode3D>& ChildNode);
	void                                            RemoveChildInternal(const std::shared_ptr<ISceneNode3D>& ChildNode);
	void                                            SetParentInternal(const std::weak_ptr<ISceneNode3D>& parentNode);


protected:
	virtual void									UpdateLocalTransform();
	virtual void									UpdateWorldTransform();
	virtual void									ForceRecalculateLocalTransform();
	IBaseManager&                                   GetBaseManager() const;

	std::shared_ptr<IColliderComponent3D>			m_Components_Collider;
	std::shared_ptr<IModelsComponent3D>				m_Components_Models;
	std::shared_ptr<ILightComponent3D>				m_Components_Light;

private:
	SceneNodeType                                   m_Type;
	std::string                                     m_Name;

	Node3DList                                      m_Children;
	Node3DNameMap                                   m_ChildrenByName;
	std::weak_ptr<ISceneNode3D>                     m_ParentNode;

	std::shared_ptr<IActionsGroup>                  m_ActionsGroup;
	std::shared_ptr<IPropertiesGroup>               m_PropertiesGroup;
	std::weak_ptr<IScene>                           m_Scene;

	vec3											m_Translate;
	vec3											m_Rotate;
	quat											m_RotateQuat;
	bool											m_IsRotateQuat;
	vec3											m_Scale;

	glm::mat4										m_LocalTransform;
	glm::mat4										m_InverseLocalTransform;
	glm::mat4										m_WorldTransform;
	glm::mat4										m_InverseWorldTransform;

	ComponentsMap                                   m_Components;
};
