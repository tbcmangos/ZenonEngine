#pragma once

class ZN_API BlendStateBase 
	: public IBlendState
{
public:
	BlendStateBase();
    virtual ~BlendStateBase();

	// IBlendState
	virtual void SetBlendMode(const BlendMode& blendMode);
	virtual void SetBlendModes(const std::vector<BlendMode>& blendModes);
	virtual const std::vector<BlendMode>& GetBlendModes() const;

	virtual void SetConstBlendFactor(const glm::vec4& constantBlendFactor);
	virtual const glm::vec4& GetConstBlendFactor() const;

    virtual void SetSampleMask(uint32_t sampleMask);
	virtual uint32_t GetSampleMask() const;

	virtual void SetAlphaCoverage(bool enabled);
	virtual bool GetAlphaCoverage() const;

	virtual void SetIndependentBlend(bool enabled);
	virtual bool GetIndependentBlend() const;

protected:
    typedef std::vector<BlendMode> BlendModeList;
    BlendModeList m_BlendModes;

    bool m_bAlphaToCoverageEnabled;
    bool m_bIndependentBlendEnabled;
    uint32_t m_SampleMask;

	glm::vec4 m_ConstBlendFactor;

    bool m_bDirty;
};
