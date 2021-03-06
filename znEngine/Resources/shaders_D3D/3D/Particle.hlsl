#include "IDB_SHADER_COMMON_INCLUDE"


struct Particle
{
	float3 Position;
	float __padding0;
	//--------------------------------------------------------------( 16 bytes )

	float2 TexCoordBegin;
	float2 TexCoordEnd;
	//--------------------------------------------------------------( 16 bytes )

	float4 Color;
	//--------------------------------------------------------------( 16 bytes )

	float2 Size;
	float __padding1[2];
	//--------------------------------------------------------------( 16 bytes )
};



//
// Structs
//
struct VertexShaderInput
{
	uint VertexID : SV_VertexID;
};

struct GeometryShaderInput
{
	uint VertexID : POSITION;
};

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float4 color    : COLOR0;
};



//
// Uniforms
//
cbuffer Material                     : register(b2)
{
	float4 DiffuseColor0;
};

Texture2D DiffuseTexture             : register(t0 );
StructuredBuffer<Particle> Particles : register(t10);



GeometryShaderInput VS_main(VertexShaderInput VertexIN)
{
	GeometryShaderInput ghi;
	ghi.VertexID = VertexIN.VertexID;
	return ghi;
}

float3 TransformPoint(const float3 Point, const float4x4 Matrix)
{
	return mul(Matrix, float4(Point, 1.0f)).xyz;
}

float3 TransformPoint2(const float3 Point, const float4x4 Matrix)
{
	return mul(Matrix, float4(Point, 0.0f)).xyz;
}

float3 ExtractScaleMatrix(const float4x4 Matrix)
{
	float3 scale;
	scale.x = length(float3(Matrix[0][0], Matrix[0][1], Matrix[0][2])); // 1st column
	scale.y = length(float3(Matrix[1][0], Matrix[1][1], Matrix[1][2])); // 2nd column
	scale.z = length(float3(Matrix[2][0], Matrix[2][1], Matrix[2][2])); // 3rd columt
	return scale;
}

[maxvertexcount(4)]
void GS_Billboard(point GeometryShaderInput input[1], inout TriangleStream<PixelShaderInput> OutputStream)
{
	Particle p = Particles[input[0].VertexID];

	float3 transformePosition = TransformPoint(p.Position, PO.Model);

	float3 planeNormal = transformePosition - GetCameraPosition();
	planeNormal = normalize(planeNormal);

	float3 upVector = GetCameraUp();

	float3 rightVector = normalize(cross(planeNormal, upVector));
	rightVector *= (p.Size.x / 2.0f);
	upVector *= (p.Size.y / 2.0f);

	float scale = length(ExtractScaleMatrix(PO.Model));
	rightVector *= ExtractScaleMatrix(PO.Model).x * scale.x;
	upVector *= ExtractScaleMatrix(PO.Model).y * scale.x;

	// Create the billboards quad
	float3 vert[4];
	vert[0] = transformePosition - rightVector - upVector; // Get bottom left vertex
	vert[1] = transformePosition + rightVector - upVector; // Get bottom right vertex
	vert[2] = transformePosition - rightVector + upVector; // Get top left vertex
	vert[3] = transformePosition + rightVector + upVector; // Get top right vertex

	// Get billboards texture coordinates
	float2 texCoord[4];
	texCoord[0] = float2(p.TexCoordBegin.x, p.TexCoordEnd.y);
	texCoord[1] = float2(p.TexCoordEnd.x,   p.TexCoordEnd.y);
	texCoord[2] = float2(p.TexCoordBegin.x, p.TexCoordBegin.y);
	texCoord[3] = float2(p.TexCoordEnd.x,   p.TexCoordBegin.y);

	//const float4x4 mv = mul(PF.View, PO.Model);
	const float4x4 mvp = mul(PF.Projection, PF.View);

	// Now we "append" or add the vertices to the outgoing stream list
	PixelShaderInput outputVert;
	for (int i = 0; i < 4; i++)
	{
		outputVert.position = mul(mvp, float4(vert[i], 1.0f));
		outputVert.texcoord = texCoord[i];
		outputVert.color = p.Color;

		OutputStream.Append(outputVert);
	}
}

DefferedRenderPSOut PS_main(PixelShaderInput input) : SV_TARGET
{
	float4 DiffuseColor = DiffuseTexture.Sample(LinearClampSampler, float2(input.texcoord.x, 1.0f - input.texcoord.y));
	//if (DiffuseColor.a < 0.01)
	//	DiffuseColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	DefferedRenderPSOut OUT;
	OUT.Diffuse = DiffuseColor * input.color;
	OUT.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	OUT.NormalWS = float4(1.0f, 1.0f, 1.0f, 0.0f);
	return OUT;
}