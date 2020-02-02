#pragma once

class ZN_API MaterialDebug : public MaterialProxie
{
public:
	MaterialDebug(IRenderDevice& RenderDevice);
	virtual ~MaterialDebug();

	cvec4 GetDiffuseColor() const;
	void SetDiffuseColor(cvec4 diffuse);

	void UpdateConstantBuffer() const override;

private:
	__declspec(align(16)) struct MaterialProperties
	{
		MaterialProperties()
			: m_DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f)
		{}
		vec4   m_DiffuseColor;
		//-------------------------- ( 16 bytes )
	};
	MaterialProperties* m_pProperties;
};