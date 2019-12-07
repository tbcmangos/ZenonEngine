#pragma once

#include "BasePass.h"

class Texture;

class PostprocessPass : public BasePass
{
public:
    typedef BasePass base;

    PostprocessPass( std::shared_ptr<IScene> scene, std::shared_ptr<IPipelineState> pipeline, const glm::mat4& projectionMatrix, std::shared_ptr<ITexture> texture );

    // Render the pass. This should only be called by the RenderTechnique.
    virtual void Render( RenderEventArgs& e );

    virtual bool Visit( SceneNode3D& node );

protected:

private:
    glm::mat4 m_ProjectionMatrix;
    std::shared_ptr<ITexture> m_Texture;
};