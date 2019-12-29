#include "IDB_SHADER_COMMON_TYPES"
#include "IDB_SHADER_COMMON_INCLUDE"

struct VertexShaderInput
{
	float3 position : POSITION;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
	float3 normal   : NORMAL;
	float2 texCoord : TEXCOORD0;
};

struct VertexShaderOutput
{
	float3 positionVS   : TEXCOORD0;    // View space position.
	float2 texCoord     : TEXCOORD1;    // Texture coordinate
	float3 tangentVS    : TANGENT;      // View space tangent.
	float3 binormalVS   : BINORMAL;     // View space binormal.
	float3 normalVS     : NORMAL;       // View space normal.
	float4 position     : SV_POSITION;  // Clip space position.
};

cbuffer PerObject : register(b0)
{
	float4x4 Model;
	float4x4 View;
	float4x4 Projection;
}
cbuffer Material : register(b2)
{
	Material Mat;
};

Texture2D AmbientTexture        : register(t0);
Texture2D EmissiveTexture       : register(t1);
Texture2D DiffuseTexture        : register(t2);
Texture2D SpecularTexture       : register(t3);
Texture2D SpecularPowerTexture  : register(t4);
Texture2D NormalTexture         : register(t5);
Texture2D BumpTexture           : register(t6);
Texture2D OpacityTexture        : register(t7);

StructuredBuffer<Light> Lights : register(t8);

sampler LinearRepeatSampler     : register(s0);
sampler LinearClampSampler      : register(s1);

VertexShaderOutput VS_main(VertexShaderInput IN)
{
	const float4x4 mvp = mul(Projection, mul(View, Model));
	const float4x4 mv = mul(View, Model);

	VertexShaderOutput OUT;
	OUT.position = mul(mvp, float4(IN.position, 1.0f));
	OUT.positionVS = mul(mv, float4(IN.position, 1.0f)).xyz;
	OUT.tangentVS = mul((float3x3)mv, IN.tangent);
	OUT.binormalVS = mul((float3x3)mv, IN.binormal);
	OUT.normalVS = mul((float3x3)mv, IN.normal);
	OUT.texCoord = IN.texCoord;
	return OUT;
}

PixelShaderOutput PS_main(VertexShaderOutput IN) : SV_TARGET
{
	// Everything is in view space.
	float4 eyePos = { 0, 0, 0, 1 };
	Material mat = Mat;

	float4 diffuse = mat.DiffuseColor;
	if (mat.HasDiffuseTexture)
	{
		float4 diffuseTex = DiffuseTexture.Sample(LinearRepeatSampler, IN.texCoord);
		if (any(diffuse.rgb))
		{
			diffuse *= diffuseTex;
		}
		else
		{
			diffuse = diffuseTex;
		}
	}

	// By default, use the alpha from the diffuse component.
	float alpha = diffuse.a;
	if (mat.HasOpacityTexture)
	{
		// If the material has an opacity texture, use that to override the diffuse alpha.
		alpha = OpacityTexture.Sample(LinearRepeatSampler, IN.texCoord).r;
	}

	float4 ambient = mat.AmbientColor;
	if (mat.HasAmbientTexture)
	{
		float4 ambientTex = AmbientTexture.Sample(LinearRepeatSampler, IN.texCoord);
		if (any(ambient.rgb))
		{
			ambient *= ambientTex;
		}
		else
		{
			ambient = ambientTex;
		}
	}

	// Combine the global ambient term.
	//ambient *= mat.GlobalAmbient;

	float4 emissive = mat.EmissiveColor;
	if (mat.HasEmissiveTexture)
	{
		float4 emissiveTex = EmissiveTexture.Sample(LinearRepeatSampler, IN.texCoord);
		if (any(emissive.rgb))
		{
			emissive *= emissiveTex;
		}
		else
		{
			emissive = emissiveTex;
		}
	}

	if (mat.HasSpecularPowerTexture)
	{
		mat.SpecularPower = SpecularPowerTexture.Sample(LinearRepeatSampler, IN.texCoord).r * mat.SpecularScale;
	}

	float4 P = float4(IN.positionVS, 1);
	float4 N;

	// Normal mapping
	if (mat.HasNormalTexture)
	{
		// For scense with normal mapping, I don't have to invert the binormal.
		float3x3 TBN = float3x3(normalize(IN.tangentVS),
									normalize(IN.binormalVS),
									normalize(IN.normalVS));

		N = DoNormalMapping(TBN, NormalTexture, LinearRepeatSampler, IN.texCoord);
	}
	// Bump mapping
	else if (mat.HasBumpTexture)
	{
		// For most scenes using bump mapping, I have to invert the binormal.
		float3x3 TBN = float3x3(normalize(IN.tangentVS),
									normalize(-IN.binormalVS),
									normalize(IN.normalVS));

		N = DoBumpMapping(TBN, BumpTexture, LinearRepeatSampler, IN.texCoord, mat.BumpIntensity);
	}
	// Just use the normal from the model.
	else
	{
		N = normalize(float4(IN.normalVS, 0));
	}

	

	LightingResult lit = DoLighting(Lights, mat, eyePos, P, N);

	//diffuse *= float4(lit.Diffuse.rgb, 1.0f); // Discard the alpha value from the lighting calculations.

	float4 specular = 0;
	if (mat.SpecularPower > 1.0f) // If specular power is too low, don't use it.
	{
		specular = mat.SpecularColor;
		if (mat.HasSpecularTexture)
		{
			float4 specularTex = SpecularTexture.Sample(LinearRepeatSampler, IN.texCoord);
			if (any(specular.rgb))
			{
				specular *= specularTex;
			}
			else
			{
				specular = specularTex;
			}
		}
		specular *= lit.Specular;
	}

    PixelShaderOutput OUT;
	OUT.PositionWS = IN.position;
	OUT.Diffuse = diffuse;
	//OUT.Diffuse = float4((ambient + emissive + diffuse + specular).rgb, alpha * mat.Opacity);
	OUT.Specular = float4(1.0, 1.0, 1.0, 1.0);
	OUT.NormalWS = float4(1.0, 1.0, 1.0, 0.0);
	return OUT;
}