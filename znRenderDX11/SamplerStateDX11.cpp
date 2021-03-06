#include "stdafx.h"

// Genral
#include "SamplerStateDX11.h"

// FORWARD BEGIN
D3D11_FILTER DX11TranslateFilter(bool IsAnisotropicFilteringEnabled, ISamplerState::CompareMode CompareMode, ISamplerState::MinFilter MinFilter, ISamplerState::MagFilter MagFilter, ISamplerState::MipFilter MipFilter);
D3D11_TEXTURE_ADDRESS_MODE DX11TranslateWrapMode(ISamplerState::WrapMode wrapMode);
D3D11_COMPARISON_FUNC DX11TranslateComparisonFunction(ISamplerState::CompareFunc compareFunc);
// FORWARD END

SamplerStateDX11::SamplerStateDX11(IRenderDeviceDX11& RenderDeviceDX11)
	: m_RenderDeviceDX11(RenderDeviceDX11)
	, m_pSamplerState(nullptr)
{}

SamplerStateDX11::~SamplerStateDX11()
{}



//
// SamplerState 
//
void SamplerStateDX11::Bind(uint32_t ID, const IShader* shader, IShaderParameter::Type parameterType) const
{
    if (m_bIsDirty)
    {
		D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = DX11TranslateFilter(m_bIsAnisotropicFilteringEnabled, m_CompareMode, m_MinFilter, m_MagFilter, m_MipFilter);
        samplerDesc.AddressU = DX11TranslateWrapMode(m_WrapModeU);
        samplerDesc.AddressV = DX11TranslateWrapMode(m_WrapModeV);
        samplerDesc.AddressW = DX11TranslateWrapMode(m_WrapModeW);
        samplerDesc.MipLODBias = m_fLODBias;
        samplerDesc.MaxAnisotropy = m_AnisotropicFiltering;
        samplerDesc.ComparisonFunc = DX11TranslateComparisonFunction(m_CompareFunc);
        samplerDesc.BorderColor[0] = m_BorderColor.r;
        samplerDesc.BorderColor[1] = m_BorderColor.g;
        samplerDesc.BorderColor[2] = m_BorderColor.b;
        samplerDesc.BorderColor[3] = m_BorderColor.a;
        samplerDesc.MinLOD = m_fMinLOD;
        samplerDesc.MaxLOD = m_fMaxLOD;

        m_pSamplerState = NULL;
        CHECK_HR(m_RenderDeviceDX11.GetDeviceD3D11()->CreateSamplerState(&samplerDesc, &m_pSamplerState));

        m_bIsDirty = false;
    }

    ID3D11SamplerState* pSamplers[] = { m_pSamplerState };

    switch (shader->GetShaderType())
    {
        case EShaderType::VertexShader:
            m_RenderDeviceDX11.GetDeviceContextD3D11()->VSSetSamplers(ID, 1, pSamplers);
            break;
        case EShaderType::TessellationControlShader:
            m_RenderDeviceDX11.GetDeviceContextD3D11()->HSSetSamplers(ID, 1, pSamplers);
            break;
        case EShaderType::TessellationEvaluationShader:
            m_RenderDeviceDX11.GetDeviceContextD3D11()->DSSetSamplers(ID, 1, pSamplers);
            break;
        case EShaderType::GeometryShader:
            m_RenderDeviceDX11.GetDeviceContextD3D11()->GSSetSamplers(ID, 1, pSamplers);
            break;
        case EShaderType::PixelShader:
            m_RenderDeviceDX11.GetDeviceContextD3D11()->PSSetSamplers(ID, 1, pSamplers);
            break;
        case EShaderType::ComputeShader:
            m_RenderDeviceDX11.GetDeviceContextD3D11()->CSSetSamplers(ID, 1, pSamplers);
            break;
    }
}

void SamplerStateDX11::UnBind(uint32_t ID, const IShader* shader, IShaderParameter::Type parameterType) const
{
    ID3D11SamplerState* pSamplers[] = { nullptr };

    switch (shader->GetShaderType())
    {
        case EShaderType::VertexShader:
            m_RenderDeviceDX11.GetDeviceContextD3D11()->VSSetSamplers(ID, 1, pSamplers);
            break;
        case EShaderType::TessellationControlShader:
            m_RenderDeviceDX11.GetDeviceContextD3D11()->HSSetSamplers(ID, 1, pSamplers);
            break;
        case EShaderType::TessellationEvaluationShader:
            m_RenderDeviceDX11.GetDeviceContextD3D11()->DSSetSamplers(ID, 1, pSamplers);
            break;
        case EShaderType::GeometryShader:
            m_RenderDeviceDX11.GetDeviceContextD3D11()->GSSetSamplers(ID, 1, pSamplers);
            break;
        case EShaderType::PixelShader:
            m_RenderDeviceDX11.GetDeviceContextD3D11()->PSSetSamplers(ID, 1, pSamplers);
            break;
        case EShaderType::ComputeShader:
            m_RenderDeviceDX11.GetDeviceContextD3D11()->CSSetSamplers(ID, 1, pSamplers);
            break;
    }
}


//
// Translate
//
D3D11_FILTER DX11TranslateFilter(bool IsAnisotropicFilteringEnabled, ISamplerState::CompareMode CompareMode, ISamplerState::MinFilter MinFilter, ISamplerState::MagFilter MagFilter, ISamplerState::MipFilter MipFilter)
{
	D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	if (IsAnisotropicFilteringEnabled)
	{
		filter = (CompareMode == ISamplerState::CompareMode::None) ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_COMPARISON_ANISOTROPIC;
		return filter;
	}

	if (MinFilter == ISamplerState::MinFilter::MinNearest && MagFilter == ISamplerState::MagFilter::MagNearest && MipFilter == ISamplerState::MipFilter::MipNearest)
	{
		filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	}
	else if (MinFilter == ISamplerState::MinFilter::MinNearest && MagFilter == ISamplerState::MagFilter::MagNearest && MipFilter == ISamplerState::MipFilter::MipLinear)
	{
		filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	}
	else if (MinFilter == ISamplerState::MinFilter::MinNearest && MagFilter == ISamplerState::MagFilter::MagLinear && MipFilter == ISamplerState::MipFilter::MipNearest)
	{
		filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	}
	else if (MinFilter == ISamplerState::MinFilter::MinNearest && MagFilter == ISamplerState::MagFilter::MagLinear && MipFilter == ISamplerState::MipFilter::MipLinear)
	{
		filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
	}
	else if (MinFilter == ISamplerState::MinFilter::MinLinear && MagFilter == ISamplerState::MagFilter::MagNearest && MipFilter == ISamplerState::MipFilter::MipNearest)
	{
		filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
	}
	else if (MinFilter == ISamplerState::MinFilter::MinLinear && MagFilter == ISamplerState::MagFilter::MagNearest && MipFilter == ISamplerState::MipFilter::MipLinear)
	{
		filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	}
	else if (MinFilter == ISamplerState::MinFilter::MinLinear && MagFilter == ISamplerState::MagFilter::MagLinear && MipFilter == ISamplerState::MipFilter::MipNearest)
	{
		filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	}
	else if (MinFilter == ISamplerState::MinFilter::MinLinear && MagFilter == ISamplerState::MagFilter::MagLinear && MipFilter == ISamplerState::MipFilter::MipLinear)
	{
		filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	}
	else
	{
		throw std::exception("Invalid texture filter modes.");
	}

	if (CompareMode != ISamplerState::CompareMode::None)
	{
		*(reinterpret_cast<int*>(&filter)) += static_cast<int>(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT);
	}

	return filter;
}

D3D11_TEXTURE_ADDRESS_MODE DX11TranslateWrapMode(ISamplerState::WrapMode wrapMode)
{
	switch (wrapMode)
	{
		case ISamplerState::WrapMode::Repeat:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case ISamplerState::WrapMode::Clamp:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		case ISamplerState::WrapMode::Mirror:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		case ISamplerState::WrapMode::Border:
			return D3D11_TEXTURE_ADDRESS_BORDER;
	}

	throw CznRenderException("DX11TranslateWrapMode: Incorrect wrap mode.");
}

D3D11_COMPARISON_FUNC DX11TranslateComparisonFunction(ISamplerState::CompareFunc compareFunc)
{
	switch (compareFunc)
	{
		case ISamplerState::CompareFunc::Never:
			return D3D11_COMPARISON_NEVER;
		case ISamplerState::CompareFunc::Less:
			return D3D11_COMPARISON_LESS;
		case ISamplerState::CompareFunc::Equal:
			return D3D11_COMPARISON_EQUAL;
		case ISamplerState::CompareFunc::LessEqual:
			return D3D11_COMPARISON_LESS_EQUAL;
		case ISamplerState::CompareFunc::Greater:
			return D3D11_COMPARISON_GREATER;
		case ISamplerState::CompareFunc::NotEqual:
			return D3D11_COMPARISON_NOT_EQUAL;
		case ISamplerState::CompareFunc::GreaterEqual:
			return D3D11_COMPARISON_GREATER_EQUAL;
		case ISamplerState::CompareFunc::Always:
			return D3D11_COMPARISON_ALWAYS;
	}

	throw CznRenderException("DX11TranslateComparisonFunction: Incorrect compare function.");
}
