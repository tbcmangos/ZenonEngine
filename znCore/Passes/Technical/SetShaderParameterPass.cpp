#include "stdafx.h"

// General
#include "SetShaderParameterPass.h"

CSetShaderParameterPass::CSetShaderParameterPass(IRenderDevice* RenderDevice, IShaderParameter* Destination, std::function<IShaderParameterSource*(void)> FuncGetSource)
	: RenderPass(RenderDevice)
	, m_Destination(Destination)
	, m_FuncGetSource(FuncGetSource)
{
	_ASSERT(Destination != nullptr);
	_ASSERT(FuncGetSource != nullptr);
}

CSetShaderParameterPass::CSetShaderParameterPass(IRenderDevice* RenderDevice, IShaderParameter* Destination, IShaderParameterSource* Source)
	: RenderPass(RenderDevice)
	, m_Destination(Destination)
	, m_Source(Source)
{
	_ASSERT(Destination != nullptr);
	_ASSERT(Source != nullptr);
}

CSetShaderParameterPass::~CSetShaderParameterPass()
{}

void CSetShaderParameterPass::Render(RenderEventArgs& e)
{
	if (m_Source != nullptr)
		m_Destination->SetSource(m_Source);

	if (m_FuncGetSource != nullptr)
	{
		IShaderParameterSource* sps = m_FuncGetSource();
		if (sps != nullptr)
		{
			m_Destination->SetSource(sps);
		}
	}
}
