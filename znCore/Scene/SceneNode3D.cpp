#include "stdafx.h"

// General
#include "SceneNode3D.h"

#include "XML/XMLManager.h"

SceneNode3D::SceneNode3D(IScene& Scene)
	: m_Scene(Scene)

	, m_Translate(0.0f)
	, m_Rotate(glm::vec3(0.0f))
	, m_RotateQuat(glm::quat())
	, m_IsRotateQuat(false)
	, m_Scale(1.0f)

	// Transform functinal
	, m_LocalTransform(1.0f)
	, m_InverseLocalTransform(1.0f)	// This is the inverse of the local -> world transform.
	, m_WorldTransform(1.0f)
	, m_InverseWorldTransform(1.0f)
{
	m_PropertiesGroup = std::make_shared<CPropertiesGroup>("SceneNodeProperties", "Some important scene node 3d properties.");
}

SceneNode3D::~SceneNode3D()
{}



//
// ISceneNode3D
//
void SceneNode3D::Initialize()
{
	// Name properties
	{
		std::shared_ptr<CPropertyWrapped<std::string>> nameProperty = std::make_shared<CPropertyWrapped<std::string>>("Name", "Scene node name.");
		nameProperty->SetValueSetter(std::bind(&Object::SetName, this, std::placeholders::_1));
		nameProperty->SetValueGetter(std::bind(&Object::GetName, this));
		GetProperties()->AddProperty(nameProperty);
	}

	// Transform properties
	{
		std::shared_ptr<IPropertiesGroup> propertiesGroup = std::make_shared<CPropertiesGroup>("Transform", "Transorm of this 3D node. Like translation, rotation and scale.");

		std::shared_ptr<CPropertyWrapped<glm::vec3>> translateProperty = std::make_shared<CPropertyWrapped<glm::vec3>>("Translate", "Position of this node in world. Relative to parent.");
		translateProperty->SetValueSetter(std::bind(&SceneNode3D::SetTranslate, this, std::placeholders::_1));
		translateProperty->SetValueGetter(std::bind(&SceneNode3D::GetTranslation, this));
		propertiesGroup->AddProperty(translateProperty);

		std::shared_ptr<CPropertyWrapped<glm::vec3>> rotationProperty = std::make_shared<CPropertyWrapped<glm::vec3>>("Rotate", "Rotation of this node. Relative to parent.");
		rotationProperty->SetValueSetter(std::bind(&SceneNode3D::SetRotation, this, std::placeholders::_1));
		rotationProperty->SetValueGetter(std::bind(&SceneNode3D::GetRotation, this));
		propertiesGroup->AddProperty(rotationProperty);

		std::shared_ptr<CPropertyWrapped<glm::vec3>> scaleProperty = std::make_shared<CPropertyWrapped<glm::vec3>>("Scale", "Scale of this node. Relative to parent.");
		scaleProperty->SetValueSetter(std::bind(&SceneNode3D::SetScale, this, std::placeholders::_1));
		scaleProperty->SetValueGetter(std::bind(&SceneNode3D::GetScale, this));
		propertiesGroup->AddProperty(scaleProperty);

		GetProperties()->AddProperty(propertiesGroup);
	}

	// Actions
	{
		std::shared_ptr<CAction> removeAction = std::make_shared<CAction>("Remove", "Remove this node from world. this action affected on childs!");
		removeAction->SetAction([this]() -> bool {
			auto sceneNode = this->shared_from_this();
			if (sceneNode->GetScene() == nullptr)
				return false;

			sceneNode->GetScene()->RemoveChild(sceneNode->GetParent().lock(), sceneNode);
			return true;
		});

		GetProperties()->AddProperty(removeAction);
	}
}

void SceneNode3D::Finalize()
{}

void SceneNode3D::Copy(std::shared_ptr<ISceneNode3D> Destination) const
{
	Object::Copy(Destination);

	auto destCast = std::dynamic_pointer_cast<SceneNode3D>(Destination);

	destCast->m_Children.clear();
	destCast->m_ParentNode;

	destCast->m_PropertiesGroup;
	destCast->m_Scene = m_Scene;

	destCast->m_Translate = m_Translate;
	destCast->m_Rotate = m_Rotate;
	destCast->m_RotateQuat = m_RotateQuat;
	destCast->m_IsRotateQuat = m_IsRotateQuat;
	destCast->m_Scale = m_Scale;
	destCast->m_LocalTransform = m_LocalTransform;
	destCast->m_InverseLocalTransform = m_InverseLocalTransform;
	destCast->m_WorldTransform = m_WorldTransform;
	destCast->m_InverseWorldTransform = m_InverseWorldTransform;

	for (const auto& c : GetComponents())
	{
		const auto& compInOther = destCast->m_Components.find(c.first);
		_ASSERT(compInOther != destCast->m_Components.end());
		c.second->Copy(compInOther->second);
	}
}



//
// Childs functional
//
void SceneNode3D::AddChild(std::shared_ptr<ISceneNode3D> childNode)
{
	if (childNode == nullptr)
		throw CException(L"SceneNode3D: Child node must not be NULL.");

	// 1. ������� ����� � �������� �������� (�������� ����� ��� �� ���� ��������������, �������� ��� ����������� BoundingBox)
	if (!childNode->GetParent().expired())
	{
		if (auto currentChildParent = childNode->GetParent().lock())
		{
			if (currentChildParent != shared_from_this())
			{
				std::dynamic_pointer_cast<SceneNode3D>(currentChildParent)->RemoveChildInternal(childNode);
				//Log::Warn("SceneNode3D: Failed to add child to his current parent.");
				//return;
			}
		}
	}

	// 2. ��������� ����� � ������ ������� (�������� ����� ��� �� ���� ��������������, �������� ��� ����������� BoundingBox)
	this->AddChildInternal(childNode);
}

void SceneNode3D::RemoveChild(std::shared_ptr<ISceneNode3D> childNode)
{
	if (childNode == nullptr)
	{
		Log::Warn("SceneNode3D: Child node must not be NULL.");
		return;
	}

	this->RemoveChildInternal(childNode);
}

std::weak_ptr<ISceneNode3D> SceneNode3D::GetParent() const
{
	return m_ParentNode;
}

const SceneNode3D::Node3DList& SceneNode3D::GetChilds() const
{
	return m_Children;
}

void SceneNode3D::ClearChilds()
{
}

void SceneNode3D::RaiseOnParentChanged()
{
	// Don't forget about update world transform
	UpdateWorldTransform();

	for (auto c : m_Components)
	{
		c.second->OnMessage(nullptr, UUID_OnParentChanged);
	}
}

std::shared_ptr<IPropertiesGroup> SceneNode3D::GetProperties() const
{
	return m_PropertiesGroup;
}

IScene* SceneNode3D::GetScene() const
{
	return &m_Scene;
}



//
// Transform functional
//
void SceneNode3D::SetTranslate(const glm::vec3& _translate)
{
	m_Translate = _translate;
	UpdateLocalTransform();
}

void SceneNode3D::AddTranslate(const glm::vec3& Translate)
{
	SetTranslate(GetTranslation() + Translate);
}

const glm::vec3& SceneNode3D::GetTranslation() const
{
	return m_Translate;
}

void SceneNode3D::SetRotation(const glm::vec3& _rotate)
{
	m_Rotate = _rotate;
	UpdateLocalTransform();
}

const glm::vec3& SceneNode3D::GetRotation() const
{
	return m_Rotate;
}

void SceneNode3D::SetRotationQuaternion(const glm::quat& _rotate)
{
	m_RotateQuat = _rotate;
	m_IsRotateQuat = true;
	UpdateLocalTransform();
}

const glm::quat& SceneNode3D::GetRotationQuaternion() const
{
	return m_RotateQuat;
}

void SceneNode3D::SetScale(const glm::vec3& _scale)
{
	m_Scale = _scale;
	UpdateLocalTransform();
}

const glm::vec3& SceneNode3D::GetScale() const
{
	return m_Scale;
}

glm::mat4 SceneNode3D::GetLocalTransform() const
{
	return m_LocalTransform;
}

glm::mat4 SceneNode3D::GetInverseLocalTransform() const
{
	return m_InverseLocalTransform;
}

void SceneNode3D::SetLocalTransform(const glm::mat4& localTransform)
{
	m_LocalTransform = localTransform;
	m_InverseLocalTransform = glm::inverse(localTransform);

	UpdateWorldTransform();
}

glm::mat4 SceneNode3D::GetWorldTransfom() const
{
	return m_WorldTransform;
}

glm::mat4 SceneNode3D::GetInverseWorldTransform() const
{
	return m_InverseWorldTransform;
}

glm::mat4 SceneNode3D::GetParentWorldTransform() const
{
	glm::mat4 parentTransform(1.0f);
	if (auto parent = GetParent().lock())
		parentTransform = parent->GetWorldTransfom();

	return parentTransform;
}

void SceneNode3D::SetWorldTransform(const glm::mat4& worldTransform)
{
	glm::mat4 inverseParentTransform = glm::inverse(GetParentWorldTransform());

	SetLocalTransform(inverseParentTransform * worldTransform);
}



//
// Components engine
//
bool SceneNode3D::IsComponentExists(ObjectClass ComponentID) const
{
	const auto& components = GetComponents();
	return components.find(ComponentID) != components.end();
}

std::shared_ptr<ISceneNodeComponent> SceneNode3D::GetComponent(ObjectClass ComponentID) const
{
	const auto& components = GetComponents();
	const auto& component = components.find(ComponentID);
	if (component == components.end())
		return nullptr;

	return component->second;
}

std::shared_ptr<ISceneNodeComponent> SceneNode3D::AddComponent(ObjectClass ComponentID, std::shared_ptr<ISceneNodeComponent> Component)
{
	m_Components[ComponentID] = Component;
	return Component;
}

const ComponentsMap& SceneNode3D::GetComponents() const
{
	return m_Components;
}

void SceneNode3D::RaiseComponentMessage(const ISceneNodeComponent* Component, ComponentMessageType Message) const
{
	const auto& components = GetComponents();
	std::for_each(components.begin(), components.end(), [&Component, &Message](const std::pair<ObjectClass, std::shared_ptr<ISceneNodeComponent>>& ComponentMapIter) {
		ComponentMapIter.second->OnMessage(Component, Message);
	});
}
void SceneNode3D::RegisterComponents()
{
	m_Components_Models = GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<IComponentFactory>()->CreateComponentT<IModelsComponent3D>(cSceneNodeModelsComponent, *this);
	m_Components_Models = ISceneNode3D::AddComponent(m_Components_Models);

	m_Components_Collider = GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<IComponentFactory>()->CreateComponentT<IColliderComponent3D>(cSceneNodeColliderComponent, *this);
	m_Components_Collider = ISceneNode3D::AddComponent(m_Components_Collider);
}

const std::shared_ptr<IColliderComponent3D>& SceneNode3D::GetColliderComponent() const
{
	return m_Components_Collider;
}

const std::shared_ptr<IModelsComponent3D>& SceneNode3D::GetModelsComponent() const
{
	return m_Components_Models;
}



//
// Others
//
void SceneNode3D::Update(const UpdateEventArgs & e)
{
	// Do nothing...
}

void SceneNode3D::Accept(IVisitor* visitor)
{
	//if (const auto& loadableObject = dynamic_cast<ILoadable*>(this))
	//	if (loadableObject->GetState() != ILoadable::ELoadableState::Loaded)
	//		return;
	
	EVisitResult visitResult = visitor->Visit(this);

	if (visitResult & EVisitResult::AllowVisitContent)
	{
		const auto& components = GetComponents();
		std::for_each(components.begin(), components.end(), [&visitor](const std::pair<ObjectClass, std::shared_ptr<ISceneNodeComponent>>& Component) {
			if (Component.second)
				Component.second->Accept(visitor);
		});
	}

	if (visitResult & EVisitResult::AllowVisitChilds)
	{
		const auto& childs = GetChilds();
		std::for_each(childs.begin(), childs.end(), [&visitor](const std::shared_ptr<ISceneNode3D>& Child) {
			if (Child != nullptr)
				Child->Accept(visitor);
		});
	}
}




//
// IObject
//

namespace
{
	std::string ExtractClearName(const std::string& DirtyName)
	{
		char buff[256];
		int num;
		if (sscanf(DirtyName.c_str(), "%s%d", buff, &num) > 0)
			return buff;

		return DirtyName;
	}
}

void SceneNode3D::SetName(const std::string& Name)
{
	std::string resultName = Name;

	if (!GetGUID().IsEmpty())
	{
		if (auto parent = GetParent().lock())
		{
			auto childs = parent->GetChilds();
			auto childIt = childs.end();
			do
			{
				childIt = std::find_if(childs.begin(), childs.end(), [resultName](const std::shared_ptr<ISceneNode3D>& Child) -> bool {
					return resultName == Child->GetName();
				});
				if (childIt != childs.end())
					resultName = ExtractClearName(resultName) + "" + std::to_string(GetGUID().GetCounter());

			} while (childIt != childs.end());

			_ASSERT(std::find_if(childs.begin(), childs.end(), [resultName](const std::shared_ptr<ISceneNode3D>& Child) -> bool {
				return resultName == Child->GetName();
			}) == childs.end());
		}
		else
			_ASSERT(true);
	}
	
	Object::SetName(resultName);
}


//
// IObjectLoadSave
//
void SceneNode3D::Load(const std::shared_ptr<IXMLReader>& Reader)
{
	Object::Load(Reader);

	DoLoadProperties(Reader);

	if (Reader->IsChildExists("Components"))
	{
		auto componentsWriter = Reader->GetChild("Components");
		for (const auto& ch : componentsWriter->GetChilds())
		{
			auto child = GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<IComponentFactory>()->LoadComponentXML(ch, *this);
			AddComponent(child->GetGUID().GetObjectClass(), child);
		}
	}

	if (Reader->IsChildExists("Childs"))
	{
		auto childsWriter = Reader->GetChild("Childs");
		for (const auto& ch : childsWriter->GetChilds())
		{
			auto child = GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<ISceneNode3DFactory>()->LoadSceneNode3DXML(ch, GetScene());
			AddChild(child);
		}
	}
}

void SceneNode3D::Save(const std::shared_ptr<IXMLWriter>& Writer) const
{
	Object::Save(Writer);

	DoSaveProperties(Writer);

	const auto& components = GetComponents();
	if (false == components.empty())
	{
		auto componentsWriter = Writer->CreateChild("Components");
		for (const auto& ch : components)
		{
			auto childWriter = GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<IComponentFactory>()->SaveComponentXML(ch.second);
			componentsWriter->AddChild(childWriter);
		}
	}

	const auto& childs = GetChilds();
	if (false == childs.empty())
	{
		auto childsWriter = Writer->CreateChild("Childs");
		for (const auto& ch : childs)
		{
			auto childWriter = GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<ISceneNode3DFactory>()->SaveSceneNode3DXML(ch);
			childsWriter->AddChild(childWriter);
		}
	}
}


//
// ISceneNode3DInternal
//
void SceneNode3D::AddChildInternal(std::shared_ptr<ISceneNode3D> ChildNode)
{
	_ASSERT(ChildNode != nullptr);

	const auto& iter = std::find(m_Children.begin(), m_Children.end(), ChildNode);
	if (iter != m_Children.end())
		throw CException(L"This parent already has this child.");

	// Add to common list
	m_Children.push_back(ChildNode);

	std::dynamic_pointer_cast<SceneNode3D>(ChildNode)->SetParentInternal(weak_from_this());

	ChildNode->SetName(ChildNode->GetName());

	// TODO: ����� ����� �������� ������?
	ChildNode->RaiseOnParentChanged();
	GetScene()->RaiseSceneChangeEvent(ESceneChangeType::NodeAddedToParent, shared_from_this(), ChildNode);
}

void SceneNode3D::RemoveChildInternal(std::shared_ptr<ISceneNode3D> ChildNode)
{
	const auto& childListIter = std::find(m_Children.begin(), m_Children.end(), ChildNode);
	if (childListIter == m_Children.end())
		//throw CException(L"Can't remove child because don't found.");
		return;

	// Delete from list
	m_Children.erase(childListIter);

	// TODO: ���� ������������ ������ �� ����� �������� � ������ ������, �� �� ���������?

	std::dynamic_pointer_cast<SceneNode3D>(ChildNode)->SetParentInternal(std::weak_ptr<ISceneNode3D>());

	// TODO: ����� ����� �������� ������?
	ChildNode->RaiseOnParentChanged();
	GetScene()->RaiseSceneChangeEvent(ESceneChangeType::NodeRemovedFromParent, shared_from_this(), ChildNode);
}

void SceneNode3D::SetParentInternal(std::weak_ptr<ISceneNode3D> parentNode)
{
	m_ParentNode = parentNode;
}



glm::mat4 SceneNode3D::CalculateLocalTransform() const
{
	glm::mat4 localTransform(1.0f);

	localTransform = glm::translate(localTransform, m_Translate);
	if (m_IsRotateQuat)
	{
		localTransform *= glm::toMat4(m_RotateQuat);
	}
	else
	{
		localTransform = glm::rotate(localTransform, m_Rotate.x, glm::vec3(1, 0, 0));
		localTransform = glm::rotate(localTransform, m_Rotate.y, glm::vec3(0, 1, 0));
		localTransform = glm::rotate(localTransform, m_Rotate.z, glm::vec3(0, 0, 1));
	}
	localTransform = glm::scale(localTransform, m_Scale);

	return localTransform;
}

//
// Protected
//
void SceneNode3D::UpdateLocalTransform()
{
	SetLocalTransform(CalculateLocalTransform());
	RaiseComponentMessage(nullptr, UUID_OnLocalTransformChanged);
}

void SceneNode3D::UpdateWorldTransform()
{
	m_WorldTransform = GetParentWorldTransform() * m_LocalTransform;
	m_InverseWorldTransform = glm::inverse(m_WorldTransform);
	
	// After world updated, we can update all childs
	for (const auto& it : GetChilds())
		std::dynamic_pointer_cast<SceneNode3D>(it)->UpdateWorldTransform();

	RaiseComponentMessage(nullptr, UUID_OnWorldTransformChanged);
}

namespace
{

}

void SceneNode3D::DoLoadProperties(const std::shared_ptr<IXMLReader>& Reader) const
{
	auto reader = Reader->GetChild(GetProperties()->GetName());
	if (reader)
		GetProperties()->Load(reader);
}

void SceneNode3D::DoSaveProperties(const std::shared_ptr<IXMLWriter>& Writer) const
{
	CXMLManager xml;
	auto propertiesWriter = xml.CreateWriter(GetProperties()->GetName());
	GetProperties()->Save(propertiesWriter);
	
	Writer->AddChild(propertiesWriter);
}

IBaseManager& SceneNode3D::GetBaseManager() const
{
	return dynamic_cast<IBaseManagerHolder*>(GetScene())->GetBaseManager();
}
