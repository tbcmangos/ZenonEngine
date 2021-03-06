#include "stdafx.h"

// General
#include "Properties.h"

// Additional
#include "Actions.h"

CPropertiesGroup::CPropertiesGroup()
	: m_Name("CPropertiesGroupName")
	, m_Description("CPropertiesGroupDescription")
{
}

CPropertiesGroup::CPropertiesGroup(std::string Name, std::string Description)
	: m_Name(Name)
	, m_Description(Description)
{
}

CPropertiesGroup::~CPropertiesGroup()
{
}



//
// IProperty
//
std::string CPropertiesGroup::GetName() const
{
	return m_Name;
}

void CPropertiesGroup::SetName(const std::string & Name)
{
	m_Name = Name;
}

std::string CPropertiesGroup::GetDescription() const
{
	return m_Description;
}

void CPropertiesGroup::SetDescription(const std::string& Description)
{
	m_Description = Description;
}

void CPropertiesGroup::Load(const std::shared_ptr<IXMLReader>& Reader)
{
	SetName(Reader->GetName());
	//SetDescription(Reader->GetStr("Description"));

	auto propertiesReader = Reader->GetChild("Properties");
	if (propertiesReader != nullptr)
	{
		for (const auto& childReader : propertiesReader->GetChilds())
		{
			std::string name = childReader->GetName();
			auto prop = GetProperty(name);
			if (prop == nullptr)
				prop = CreatePropetyByType(childReader->GetStrAttribute("Type"));
			if (prop == nullptr)
				continue;
			prop->Load(childReader);
			AddProperty(prop);
		}
	}
}

void CPropertiesGroup::Save(const std::shared_ptr<IXMLWriter>& Writer) const
{
	Writer->SetName(GetName());
	//Writer->AddStr(GetDescription(), "Description");

	auto propertiesWriter = Writer->CreateChild("Properties");
	for (const auto& prop : GetProperties())
	{
		std::string typeName = GetPropertyTypeName(prop.second.get());
		if (typeName.empty())
			continue;
		auto propertyWriter = propertiesWriter->CreateChild(prop.first);
		propertyWriter->SetStrAttribute(typeName, "Type");
		prop.second->Save(propertyWriter);
	}
}



//
// IPropertiesGroup
//
void CPropertiesGroup::AddProperty(std::shared_ptr<IProperty> Property)
{
	m_Properties.insert(std::make_pair(Property->GetName(), Property));
}

std::shared_ptr<IProperty> CPropertiesGroup::GetProperty(const std::string& PropertyName)
{
	return m_Properties[PropertyName];
}

const std::unordered_map<std::string, std::shared_ptr<IProperty>>& CPropertiesGroup::GetProperties() const
{
	return m_Properties;
}


//
// Private
//
std::string CPropertiesGroup::GetPropertyTypeName(const IProperty* Property) const
{
	if (auto prop = dynamic_cast<const CProperty<float>*>(Property))
		return "Float";
	else if (auto prop = dynamic_cast<const CProperty<glm::vec2>*>(Property))
		return "Vec2";
	else if (auto prop = dynamic_cast<const CProperty<glm::vec3>*>(Property))
		return "Vec3";
	else if (auto prop = dynamic_cast<const CProperty<glm::vec4>*>(Property))
		return "Vec4";
	else if (auto prop = dynamic_cast<const CProperty<std::string>*>(Property))
		return "String";
	else if (auto prop = dynamic_cast<const CPropertiesGroup*>(Property))
		return "Group";
	else if (auto action = dynamic_cast<const CAction*>(Property))
		return "";
	else
		throw CException("Unknown property type '%s'", GetName().c_str());
}

std::shared_ptr<IProperty> CPropertiesGroup::CreatePropetyByType(std::string TypeName)
{
	if (TypeName == "Float")
		return std::make_shared<CProperty<float>>();
	else if (TypeName == "Vec2")
		return std::make_shared<CProperty<glm::vec2>>();
	else if (TypeName == "Vec3")
		return std::make_shared<CProperty<glm::vec3>>();
	else if (TypeName == "Vec4")
		return std::make_shared<CProperty<glm::vec4>>();
	else if (TypeName == "String")
		return std::make_shared<CProperty<std::string>>();
	else if (TypeName == "Group")
		return std::make_shared<CPropertiesGroup>();
	else if (TypeName == "")
		return nullptr;
	else
		throw CException("Unknown property type '%s'", TypeName);
}
