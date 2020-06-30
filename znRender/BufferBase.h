#pragma once

ZN_INTERFACE IBufferPrivate
{
	virtual void InitializeBufferBase(const void* data, uint32 count, uint32 offset, uint32 stride) = 0;
	virtual void DoInitializeBuffer() = 0;
};

class ZN_API CBufferBase
	: public IBuffer
	, public IObjectLoadSave
	, public IBufferPrivate
{
public:
	CBufferBase(IRenderDevice& RenderDevice, IBuffer::BufferType ByfferType);
	virtual ~CBufferBase();

protected:
	// IBuffer
	virtual bool Bind(uint32 id, const IShader* shader, IShaderParameter::Type parameterType) const = 0;
	virtual void UnBind(uint32 id, const IShader* shader, IShaderParameter::Type parameterType) const = 0;
	virtual void Copy(const IBuffer* other) const = 0;
	BufferType GetType() const override;
	uint32 GetElementCount() const override;
	uint32 GetElementStride() const override;
	uint32 GetElementOffset() const override;

	// IObjectLoadSave
	virtual void Load(const std::shared_ptr<IByteBuffer>& ByteBuffer) override;
	virtual void Save(const std::shared_ptr<IByteBuffer>& ByteBuffer) override;

	// IBufferPrivate
	void InitializeBufferBase(const void* data, uint32 count, uint32 offset, uint32 stride) override final;
	virtual void DoInitializeBuffer() = 0;

protected:
 	const std::vector<uint8>& GetData() const;
	std::vector<uint8>& GetDataEx();
	void SetData(const void* data, size_t dataSize);

private:
	BufferType m_BufferType;
	std::vector<uint8_t> m_Data;
	uint32 m_Count; // The number of elements in this buffer.
	uint32 m_Offset; // The offset of the vertex buffer in bytes.
	uint32 m_Stride; // The stride of the vertex buffer in bytes.

private: // Link to parent
	IRenderDevice& m_RenderDevice;
};