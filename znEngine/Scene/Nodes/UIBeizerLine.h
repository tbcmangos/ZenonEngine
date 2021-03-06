#pragma once

#if 0

#include "../../Materials/UI_Line_Material.h"

class ZN_API CUIBeizerLineNode 
	: public SceneNodeUI
{
	typedef SceneNodeUI base;
public:
	CUIBeizerLineNode(IRenderDevice& RenderDevice);
	virtual	~CUIBeizerLineNode();

	// CUIBeizerLineNode
	void                                            SetColor(vec4 Color);
    void                                            SetPosStart(vec2 PosStart);
    void                                            SetPosEnd(vec2 PosEnd);
    void                                            SetThickness(float Thickness);

	// SceneNodeUI
    virtual glm::vec2                               GetSize() override final;

	virtual bool                                    AcceptMesh(IVisitor* visitor) override;

protected:
    void UpdateBuffer();

private:
	std::shared_ptr<IModel>              m_Mesh;
    std::shared_ptr<IBuffer>            m_PointsBuffer;
	std::shared_ptr<UI_Line_Material>   m_Material;

    vec2                                m_StartPoint;
    vec2                                m_EndPoint;
};

#endif

