#pragma once


#include "Passes/DefferedRender.h"
#include "Passes/DefferedRenderPrepareLights.h"
#include "Passes/DefferedRenderFinal.h"


#include "SceneFunctional/UI/UITexture.h"

class ZN_API CSceneDefault 
	: public SceneBase
{
public:
	CSceneDefault(IBaseManager& BaseManager);
	virtual ~CSceneDefault();

	// IGameState
	void Initialize() override;
	void Finalize() override;

	virtual void OnRayIntersected(const glm::vec3& Point);

	virtual void OnPreRender(RenderEventArgs& e) override;

	// Keyboard events
	virtual bool OnWindowKeyPressed(KeyEventArgs& e) override;
	virtual void OnWindowKeyReleased(KeyEventArgs& e) override;

private:
	void Load3D();
	void LoadUI();
	void GenerateLights(std::shared_ptr<ISceneNode3D> Node, uint32_t numLights);
	void CSceneDefault::UpdateLights();

private:
	std::shared_ptr<CUITextureNode> m_TextureUI;

private:
	std::shared_ptr<SceneNode3D> m_RootForBoxes;

	std::shared_ptr<CSceneCreateTypelessListPass> m_SceneCreateTypelessListPass;
	std::shared_ptr<CDefferedRender> m_DefferedRenderPass;
	std::shared_ptr<CDefferedRenderPrepareLights> m_DefferedRenderPrepareLights;
	std::shared_ptr<CDefferedRenderFinal> m_DefferedFinalRenderPass;
};