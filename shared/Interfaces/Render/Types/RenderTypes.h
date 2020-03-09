#pragma once

/**
 * Flags to specify which value should be cleared.
 */
enum class ZN_API ClearFlags : uint8_t
{
	Color = 1 << 0,
	Depth = 1 << 1,
	Stencil = 1 << 2,
	DepthStencil = Depth | Stencil,
	All = Color | Depth | Stencil,
};


/**
  * CPU Access. Used for textures and Buffers
 */
enum class ZN_API CPUAccess : uint8_t
{
	None = 0,                 // No CPU access to this texture is necessary.
	Read = (1 << 0),          // CPU reads permitted.
	Write = (1 << 1),         // CPU writes permitted.
	ReadWrite = Read | Write
};


enum class ZN_API EShaderType : uint8_t
{
	UnknownShaderType = 0,
	VertexShader,
	TessellationControlShader,      // Hull Shader in DirectX
	TessellationEvaluationShader,   // Domain Shader in DirectX
	GeometryShader,
	PixelShader,
	ComputeShader,
};

/**
 * Used for meshes
 */
enum class ZN_API PrimitiveTopology : uint8_t
{
	PointList,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip
};


struct ZN_API BufferBinding
{
	BufferBinding()
		: Index(0)
	{}
	BufferBinding(const std::string& name, uint32 index)
		: Name(name)
		, Index(index)
	{}
	virtual ~BufferBinding()
	{}

	inline bool operator==(const BufferBinding& rhs) const
	{
		return (Name == rhs.Name) && (Index == rhs.Index);
	}

	std::string  Name;
	uint32       Index;
};

namespace std
{
	template<>
	struct hash<BufferBinding>
	{
		size_t operator()(const BufferBinding& buffer) const noexcept
		{
			std::hash<std::string> hash;
			return hash(buffer.Name + std::to_string(buffer.Index));
		}
	};
}


struct ZN_API SGeometryDrawArgs
{
	SGeometryDrawArgs(UINT IndexStartLocation = 0, UINT IndexCnt = UINT_MAX, UINT VertexStartLocation = 0, UINT VertexCnt = UINT_MAX)
		: IndexStartLocation(IndexStartLocation)
		, IndexCnt(IndexCnt)
		, VertexStartLocation(VertexStartLocation)
		, VertexCnt(VertexCnt)
		, InstanceStartIndex(0)
		, InstanceCnt(UINT_MAX)
	{}

	UINT IndexStartLocation;
	UINT IndexCnt;
	UINT VertexStartLocation;
	UINT VertexCnt;
	UINT InstanceStartIndex;
	UINT InstanceCnt;
};