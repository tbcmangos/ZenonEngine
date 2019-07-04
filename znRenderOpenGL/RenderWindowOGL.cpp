#include <stdafx.h>

// Include
#include "RenderDeviceOGL.h"

// General
#include "RenderWindowOGL.h"

// Additional
#include "RenderTargetOGL.h"
#include "TextureOGL.h"

RenderWindowOGL::RenderWindowOGL(std::shared_ptr<RenderDeviceOGL> RenderDevice, IWindowObject * WindowObject, bool vSync)
	: RenderWindow(RenderDevice, WindowObject, vSync)
{
	m_HDC = GetDC(GetHWnd());

    std::shared_ptr<RenderDeviceOGL> renderDeviceDX11 = std::dynamic_pointer_cast<RenderDeviceOGL>(GetRenderDevice());
    renderDeviceDX11->InitDevice(m_HDC);

    CreateSwapChain();
}

RenderWindowOGL::~RenderWindowOGL()
{
}


void RenderWindowOGL::Present()
{
	std::shared_ptr<TextureOGL> colorBuffer = std::dynamic_pointer_cast<TextureOGL>(GetRenderTarget()->GetTexture(IRenderTarget::AttachmentPoint::Color0));
	if (colorBuffer)
	{
		//m_pDeviceContext->CopyResource(m_pBackBuffer, colorBuffer->GetTextureResource());
	}

	if (IsVSync())
	{
        if (!SwapBuffers(m_HDC))
            fail1();
	}
	else
	{
        if (!SwapBuffers(m_HDC))
            fail1();
	}
}



//
// Engine events
//
void RenderWindowOGL::OnPreRender(RenderEventArgs& e)
{
	// Get the currently bound frame buffer object to avoid reset to invalid FBO
	GLint defaultRB = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultRB);
	OGLCheckError();

	std::dynamic_pointer_cast<RenderDeviceOGL>(GetRenderDevice())->SetDefaultRB(defaultRB);

	base::OnPreRender(e);
}



//
// Protected
//
void RenderWindowOGL::CreateSwapChain()
{
    base::CreateSwapChain();

    // Create the device and swap chain before the window is shown.
    UINT windowWidth2 = GetWindowWidth();
    UINT windowHeight2 = GetWindowHeight();
    bool vSync2 = IsVSync();

    std::string windowName2 = GetWindowName();

    // Depth/stencil buffer
    Texture::TextureFormat depthStencilTextureFormat(
        Texture::Components::Depth,
        Texture::Type::UnsignedNormalized,
        1, //m_SampleDesc.Count,
        0, 0, 0, 0, 24, 0);
    std::shared_ptr<Texture> depthStencilTexture = GetRenderDevice()->CreateTexture2D(windowWidth2, windowHeight2, 1, depthStencilTextureFormat);

    // Color buffer (Color0)
    Texture::TextureFormat colorTextureFormat
    (
        Texture::Components::RGBA,
        Texture::Type::UnsignedNormalized,
        1, //m_SampleDesc.Count,
        8, 8, 8, 8, 0, 0
    );
    std::shared_ptr<Texture> colorTexture = GetRenderDevice()->CreateTexture2D(windowWidth2, windowHeight2, 1, colorTextureFormat);

    GetRenderTarget()->AttachTexture(IRenderTarget::AttachmentPoint::Color0, colorTexture);
    GetRenderTarget()->AttachTexture(IRenderTarget::AttachmentPoint::Depth, depthStencilTexture);
    GetRenderTarget()->Resize(windowWidth2, windowHeight2);
}

void RenderWindowOGL::ResizeSwapChainBuffers(uint32_t width, uint32_t height)
{
    width = glm::max<uint32_t>(width, 1);
    height = glm::max<uint32_t>(height, 1);

    GetRenderTarget()->Resize(width, height);
}
