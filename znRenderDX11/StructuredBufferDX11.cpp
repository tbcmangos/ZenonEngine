#include "stdafx.h"

#include "StructuredBufferDX11.h"

StructuredBufferDX11::StructuredBufferDX11(IRenderDeviceDX11& RenderDeviceDX11, UINT bindFlags, const void* data, size_t count, UINT stride, CPUAccess cpuAccess, bool bUAV)
	: m_RenderDeviceDX11(RenderDeviceDX11)
	, m_uiStride(stride)
	, m_uiCount((UINT)count)
	, m_BindFlags(bindFlags)
	, m_bIsDirty(false)
	, m_CPUAccess(cpuAccess)
{
	m_bDynamic = (int)m_CPUAccess != 0;
	// Dynamic buffers cannot also be UAV's
	m_bUAV = bUAV && !m_bDynamic;

	// Assign the data to the system buffer.
	size_t numBytes = m_uiCount * m_uiStride;

	if (data)
	{
		m_Data.assign((uint8_t*)data, (uint8_t*)data + numBytes);
	}
	else
	{
		m_Data.reserve(numBytes);
	}

	// Create a GPU buffer to store the data.
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = (UINT)numBytes;

	if (((int)m_CPUAccess & (int)CPUAccess::Read) != 0)
	{
		bufferDesc.Usage = D3D11_USAGE_STAGING;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}
	else if (((int)m_CPUAccess & (int)CPUAccess::Write) != 0)
	{
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}
	else
	{
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (m_bUAV)
		{
			bufferDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		}
	}

	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = m_uiStride;

	D3D11_SUBRESOURCE_DATA subResourceData;
	subResourceData.pSysMem = (void*)m_Data.data();
	subResourceData.SysMemPitch = 0;
	subResourceData.SysMemSlicePitch = 0;

	if (FAILED(m_RenderDeviceDX11.GetDeviceD3D11()->CreateBuffer(&bufferDesc, &subResourceData, &m_pBuffer)))
	{
		_ASSERT_EXPR(false, "Failed to create read/write buffer.");
		return;
	}

	if ((bufferDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE) != 0)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = m_uiCount;

		if (FAILED(m_RenderDeviceDX11.GetDeviceD3D11()->CreateShaderResourceView(m_pBuffer, &srvDesc, &m_pSRV)))
		{
			_ASSERT_EXPR(false, "Failed to create shader resource view.");
			return;
		}
	}

	if ((bufferDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS) != 0)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = m_uiCount;
		uavDesc.Buffer.Flags = 0;

		if (FAILED(m_RenderDeviceDX11.GetDeviceD3D11()->CreateUnorderedAccessView(m_pBuffer, &uavDesc, &m_pUAV)))
		{
			_ASSERT_EXPR(false, "Failed to create unordered access view.");
			return;
		}
	}
}

StructuredBufferDX11::~StructuredBufferDX11()
{}


//
// IBuffer
//
bool StructuredBufferDX11::Bind(uint32 ID, const IShader* shader, IShaderParameter::Type parameterType) const
{
	if (m_bIsDirty)
	{
		Commit();
		m_bIsDirty = false;
	}

	if (parameterType == IShaderParameter::Type::Buffer && m_pSRV)
	{
		ID3D11ShaderResourceView* srv[] = { m_pSRV };

		switch (shader->GetType())
		{
		case EShaderType::VertexShader:
			m_RenderDeviceDX11.GetDeviceContextD3D11()->VSSetShaderResources(ID, 1, srv);
			break;
		case EShaderType::TessellationControlShader:
			m_RenderDeviceDX11.GetDeviceContextD3D11()->HSSetShaderResources(ID, 1, srv);
			break;
		case EShaderType::TessellationEvaluationShader:
			m_RenderDeviceDX11.GetDeviceContextD3D11()->DSSetShaderResources(ID, 1, srv);
			break;
		case EShaderType::GeometryShader:
			m_RenderDeviceDX11.GetDeviceContextD3D11()->GSSetShaderResources(ID, 1, srv);
			break;
		case EShaderType::PixelShader:
			m_RenderDeviceDX11.GetDeviceContextD3D11()->PSSetShaderResources(ID, 1, srv);
			break;
		case EShaderType::ComputeShader:
			m_RenderDeviceDX11.GetDeviceContextD3D11()->CSSetShaderResources(ID, 1, srv);
			break;
		}
	}
	else if (parameterType == IShaderParameter::Type::RWBuffer && m_pUAV)
	{
		ID3D11UnorderedAccessView* uav[] = { m_pUAV };
		switch (shader->GetType())
		{
		case EShaderType::ComputeShader:
			m_RenderDeviceDX11.GetDeviceContextD3D11()->CSSetUnorderedAccessViews(ID, 1, uav, nullptr);
			break;
		}
	}

	return true;
}

void StructuredBufferDX11::UnBind(uint32 ID, const IShader* shader, IShaderParameter::Type parameterType) const
{
	ID3D11UnorderedAccessView* uav[] = { nullptr };
	ID3D11ShaderResourceView* srv[] = { nullptr };

	if (parameterType == IShaderParameter::Type::Buffer)
	{
		switch (shader->GetType())
		{
		case EShaderType::VertexShader:
			m_RenderDeviceDX11.GetDeviceContextD3D11()->VSSetShaderResources(ID, 1, srv);
			break;
		case EShaderType::TessellationControlShader:
			m_RenderDeviceDX11.GetDeviceContextD3D11()->HSSetShaderResources(ID, 1, srv);
			break;
		case EShaderType::TessellationEvaluationShader:
			m_RenderDeviceDX11.GetDeviceContextD3D11()->DSSetShaderResources(ID, 1, srv);
			break;
		case EShaderType::GeometryShader:
			m_RenderDeviceDX11.GetDeviceContextD3D11()->GSSetShaderResources(ID, 1, srv);
			break;
		case EShaderType::PixelShader:
			m_RenderDeviceDX11.GetDeviceContextD3D11()->PSSetShaderResources(ID, 1, srv);
			break;
		case EShaderType::ComputeShader:
			m_RenderDeviceDX11.GetDeviceContextD3D11()->CSSetShaderResources(ID, 1, srv);
			break;
		}
	}
	else if (parameterType == IShaderParameter::Type::RWBuffer)
	{
		switch (shader->GetType())
		{
		case EShaderType::ComputeShader:
			m_RenderDeviceDX11.GetDeviceContextD3D11()->CSSetUnorderedAccessViews(ID, 1, uav, nullptr);
			break;
		}
	}

}

void StructuredBufferDX11::Copy(const IBuffer* other) const
{
	Copy(dynamic_cast<const IStructuredBuffer*>(other));
}

IBuffer::BufferType StructuredBufferDX11::GetType() const
{
	return IBuffer::BufferType::StructuredBuffer;
}

uint32 StructuredBufferDX11::GetElementCount() const
{
	return m_uiCount;
}

uint32 StructuredBufferDX11::GetElementStride() const
{
	return m_uiStride;
}

uint32 StructuredBufferDX11::GetElementOffset() const
{
    return 0;
}



//
// IStructuredBuffer
//
void StructuredBufferDX11::Copy(const IStructuredBuffer* other)
{
	const StructuredBufferDX11* srcBuffer = dynamic_cast<const StructuredBufferDX11*>(other);

	if (srcBuffer->m_bIsDirty)
	{
		srcBuffer->Commit();
	}

	if (srcBuffer && srcBuffer != this && m_uiCount * m_uiStride == srcBuffer->m_uiCount * srcBuffer->m_uiStride)
	{
		m_RenderDeviceDX11.GetDeviceContextD3D11()->CopyResource(m_pBuffer, srcBuffer->m_pBuffer);
	}
	else
	{
		Log::Error("Source buffer is not compatible with this buffer.");
	}

	if (((uint8_t)m_CPUAccess & (uint8_t)CPUAccess::Read) != 0)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (FAILED(m_RenderDeviceDX11.GetDeviceContextD3D11()->Map(m_pBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
		{
			Log::Error("Failed to map texture resource for reading.");
		}

		memcpy_s(m_Data.data(), m_Data.size(), mappedResource.pData, m_Data.size());

		m_RenderDeviceDX11.GetDeviceContextD3D11()->Unmap(m_pBuffer, 0);
	}
}

void StructuredBufferDX11::Set(void* data, size_t elementSize, size_t offset, size_t numElements)
{
	unsigned char* first = (unsigned char*)data + (offset * elementSize);
	unsigned char* last = first + (numElements * elementSize);
	m_Data.assign(first, last);

	m_bIsDirty = true;
}

void StructuredBufferDX11::Clear()
{
	if (m_pUAV)
	{
		FLOAT clearColor[4] = { 0, 0, 0, 0 };
		m_RenderDeviceDX11.GetDeviceContextD3D11()->ClearUnorderedAccessViewFloat(m_pUAV, clearColor);
	}
}



//
// IStructuredBufferDX11
//
ID3D11UnorderedAccessView* StructuredBufferDX11::GetUnorderedAccessView() const
{
	return m_pUAV;
}



//
// Protected
//
void StructuredBufferDX11::Commit() const
{
	if (m_bIsDirty && m_bDynamic && m_pBuffer)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (FAILED(m_RenderDeviceDX11.GetDeviceContextD3D11()->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			Log::Error("Failed to map subresource.");
		}

		memcpy_s(mappedResource.pData, m_Data.size(), m_Data.data(), m_Data.size());

		m_RenderDeviceDX11.GetDeviceContextD3D11()->Unmap(m_pBuffer, 0);

		m_bIsDirty = false;
	}
}