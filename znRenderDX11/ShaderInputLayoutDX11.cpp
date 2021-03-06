#include "stdafx.h"

// General
#include "ShaderInputLayoutDX11.h"

// Additional
#include "D3D9_To_D3D11.h"


static InputSemantic gs_InvalidShaderSemantic;


// FORWARD BEGIN
DXGI_FORMAT GetDXGIFormat(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc);
// FORWARD END


ShaderInputLayoutDX11::ShaderInputLayoutDX11(IRenderDeviceDX11& RenderDeviceDX11)
	: m_RenderDeviceDX11(RenderDeviceDX11)
{
}

ShaderInputLayoutDX11::~ShaderInputLayoutDX11()
{
	m_pInputLayout.Release();
}



//
// IShaderInputLayout
//
bool ShaderInputLayoutDX11::HasSemantic(const BufferBinding & binding) const
{
	for (const auto& it : m_InputSemantics)
		if (it.first.Name == binding.Name && it.first.Index == binding.Index)
			return true;

	return false;
}

const InputSemantic& ShaderInputLayoutDX11::GetSemantic(const BufferBinding & binding) const
{
	for (auto& it : m_InputSemantics)
		if (it.first.Name == binding.Name && it.first.Index == binding.Index)
			return it.first;

	return gs_InvalidShaderSemantic;
}

UINT ShaderInputLayoutDX11::GetSemanticSlot(const BufferBinding & binding) const
{
	for (auto& it : m_InputSemantics)
		if ((strcmp(it.first.Name.c_str(), binding.Name.c_str()) == 0) && it.first.Index == binding.Index)
			return it.second;

	return UINT_MAX;
}



//
// ShaderInputLayoutDX11
//
bool ShaderInputLayoutDX11::LoadFromReflector(ID3DBlob * pShaderBlob, ID3D11ShaderReflection * pReflector)
{
	// Query input parameters and build the input layout
	D3D11_SHADER_DESC shaderDescription = {};
	CHECK_HR(pReflector->GetDesc(&shaderDescription), L"Failed to get shader description from shader reflector.");

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
	for (UINT i = 0; i < shaderDescription.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC parameterSignature = {};
		CHECK_HR(pReflector->GetInputParameterDesc(i, &parameterSignature));

		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName = parameterSignature.SemanticName;
		inputElement.SemanticIndex = parameterSignature.SemanticIndex;
		inputElement.InputSlot = i; // TODO: If using interleaved arrays, then the input slot should be 0.  If using packed arrays, the input slot will vary.
		inputElement.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		inputElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // TODO: Figure out how to deal with per-instance data? .. Don't. Just use structured buffers to store per-instance data and use the SV_InstanceID as an index in the structured buffer.
		inputElement.InstanceDataStepRate = 0;
		inputElement.Format = GetDXGIFormat(parameterSignature);
		inputElements.push_back(inputElement);

		_ASSERT(inputElement.Format != DXGI_FORMAT_UNKNOWN);

		m_InputSemantics.insert(SemanticMap::value_type(InputSemantic(inputElement.SemanticName, inputElement.SemanticIndex), i));
	}

	if (inputElements.size() > 0)
	{
		CHECK_HR_MSG(m_RenderDeviceDX11.GetDeviceD3D11()->CreateInputLayout(inputElements.data(), (UINT)inputElements.size(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &m_pInputLayout), L"Failed to create input layout.");
	}

	return true;
}

bool ShaderInputLayoutDX11::LoadFromCustomElements(ID3DBlob * pShaderBlob, const std::vector<SCustomVertexElement>& CustomElements)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
	ConvertVertexDeclaration(CustomElements, inputElements);

	for (uint32 i = 0; i < inputElements.size() - 1; i++)
	{
		m_InputSemantics.insert(SemanticMap::value_type(InputSemantic(inputElements[i].SemanticName, inputElements[i].SemanticIndex), i));
	}

	if (inputElements.size() > 0)
	{
		CHECK_HR_MSG(m_RenderDeviceDX11.GetDeviceD3D11()->CreateInputLayout(inputElements.data(), (UINT)inputElements.size() - 1, pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &m_pInputLayout), L"Failed to create input layout.");
	}

	return true;
}

//
// ShaderInputLayoutDX11
//
ID3D11InputLayout* ShaderInputLayoutDX11::GetInputLayout() const
{
	return m_pInputLayout;
}



//
// HELPERS
//
DXGI_FORMAT GetDXGIFormat(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc)
{
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	if (paramDesc.Mask == 1) // 1 component
	{
		switch (paramDesc.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
		{
			format = DXGI_FORMAT_R32_UINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_SINT32:
		{
			format = DXGI_FORMAT_R32_SINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
		{
			format = DXGI_FORMAT_R32_FLOAT;
		}
		break;
		}
	}
	else if (paramDesc.Mask <= 3) // 2 components
	{
		switch (paramDesc.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
		{
			format = DXGI_FORMAT_R32G32_UINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_SINT32:
		{
			format = DXGI_FORMAT_R32G32_SINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
		{
			format = DXGI_FORMAT_R32G32_FLOAT;
		}
		break;
		}
	}
	else if (paramDesc.Mask <= 7) // 3 components
	{
		switch (paramDesc.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
		{
			format = DXGI_FORMAT_R32G32B32_UINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_SINT32:
		{
			format = DXGI_FORMAT_R32G32B32_SINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
		{
			format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		break;
		}
	}
	else if (paramDesc.Mask <= 15) // 4 components
	{
		switch (paramDesc.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
		{
			format = DXGI_FORMAT_R32G32B32A32_UINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_SINT32:
		{
			format = DXGI_FORMAT_R32G32B32A32_SINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
		{
			format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		break;
		}
	}

	return format;
}
