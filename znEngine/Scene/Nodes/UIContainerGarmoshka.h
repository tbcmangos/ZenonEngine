#pragma once

#ifndef ONLY_SIMPLE_UI

// Common UI
#include "SceneFunctional/UI/SceneNodeUI.h"

// ContainerGarmoshka
#include "UIContainerGarmoshkaCategory.h"

class CUIContainerGarmoshka : public SceneNodeUI
{
public:
    CUIContainerGarmoshka();
    virtual ~CUIContainerGarmoshka();

    //
    // CUIContainerGarmoshka
    //
    void Initialize(vec2 Size);
    void CreateDefault();

    std::shared_ptr<CUIContainerGarmoshkaCategory> CreateCategory(const std::string& CategoryName);

    // Open & close some categories
    void OnCategoryHeaderClicked(std::shared_ptr<CUIContainerGarmoshkaCategory> Category);


    //
    // SceneNodeUI
    //
    glm::vec2 GetSize() const override final;
    std::vector<std::shared_ptr<SceneNodeUI>> GetChilds() const override;

protected:
    void CalculateActiveCategory();
    void CalculateCategoriesTranslate();

private:
    std::shared_ptr<CUIColorNode>                               m_Background;


    std::vector<std::shared_ptr<CUIContainerGarmoshkaCategory>> m_Categories;
    std::shared_ptr<CUIContainerGarmoshkaCategory>              m_ActiveCategory;
};

#endif
