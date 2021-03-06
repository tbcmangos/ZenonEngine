#pragma once

#include "ObjectsFactories/Object.h"

class ZN_API MaterialProxie 
	: public IMaterial
	, protected IMaterialDataOwner
	, public Object
{
public:
	MaterialProxie(std::shared_ptr<IMaterial> _materal);
	virtual ~MaterialProxie() override;

	// IMaterial
	virtual void SetTexture(uint8 type, const std::shared_ptr<ITexture> texture) override;
	virtual const std::shared_ptr<ITexture>& GetTexture(uint8 ID) const override;
	
	virtual void SetSampler(uint8 ID, const std::shared_ptr<ISamplerState> samplerState) override;
    virtual const std::shared_ptr<ISamplerState>& GetSampler(uint8 ID) const override;
    
	virtual void Bind(const ShaderMap& shaders) const override;
	virtual void Unbind(const ShaderMap& shaders) const override;

	// IObject
	Guid GetGUID() const override final { return Object::GetGUID(); };
	std::string GetName() const override final { return Object::GetName(); };
	void SetName(const std::string& Name) override final { Object::SetName(Name); };

	// IObjectLoadSave
	void Load(const std::shared_ptr<IByteBuffer>& Buffer) override;
	void Save(const std::shared_ptr<IByteBuffer>& Buffer) const override;
	void Load(const std::shared_ptr<IXMLReader>& Reader) override;
	void Save(const std::shared_ptr<IXMLWriter>& Writer) const override;

protected:
	// IMaterialDataOwner
	void InitializeMaterialData(size_t BufferSize) override;
	const void* GetMaterialData() const override;
	void* GetMaterialDataEx() override;
	void MarkMaterialDataDirty() override;

private:
	std::shared_ptr<IMaterial> m_Material;
};