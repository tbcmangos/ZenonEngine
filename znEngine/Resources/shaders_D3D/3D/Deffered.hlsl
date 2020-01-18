#include "IDB_SHADER_COMMON_TYPES"
#include "IDB_SHADER_COMMON_INCLUDE"

struct VS_Input
{
	float2 position : POSITION;
	float2 texCoord : TEXCOORD0;
};

struct VS_Output
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};



// b0 - per object position

cbuffer PerFrame : register(b1)
{
	PerFrame PF;
}

// b2 - material
// b3 - ScreenParams

cbuffer LightResult : register(b4)
{
	Light L;
	float4x4 LightViewMatrix;
	float4x4 LightProjectionMatrix;

	bool IsShadowEnabled;
};

Texture2D Texture0            : register(t0); // Diffuse
Texture2D Texture1            : register(t1); // Specular
Texture2D Texture2            : register(t2); // Position
Texture2D Texture3            : register(t3); // Normal
Texture2D TextureDepthStencil : register(t4); // DepthStencil

Texture2D TextureShadow       : register(t5);

VS_Output VS_ScreenQuad(VS_Input IN)
{
	VS_Output VSOut;
	VSOut.position = float4(IN.position, 0.0f, 1.0f);
	VSOut.texCoord = IN.texCoord;
	return VSOut;
}

[earlydepthstencil]
float4 PS_ScreenQuad(VS_Output VSOut) : SV_TARGET
{
	return Texture0.Sample(LinearRepeatSampler, VSOut.texCoord) /** Texture1.Sample(TextureSampler, VSOut.texCoord) * Texture2.Sample(TextureSampler, VSOut.texCoord) * Texture3.Sample(TextureSampler, VSOut.texCoord)*/;
}

[earlydepthstencil]
float4 PS_DeferredLighting(VS_Output VSOut) : SV_Target
{
	// Everything is in view space.
	float4 eyePos = { 0, 0, 0, 1 };

	int2 texCoord = VSOut.position.xy;
	float depth = TextureDepthStencil.Load(int3(texCoord, 0)).r;

	float4 P = ScreenToView(float4(texCoord, depth, 1.0f));

	// View vector
	float4 V = normalize(eyePos - P);

	float4 diffuse        = Texture0.Load(int3(texCoord, 0));
	float4 specular       = Texture1.Load(int3(texCoord, 0));
	float4 nativePosition = Texture2.Load(int3(texCoord, 0));
	float4 N              = Texture3.Load(int3(texCoord, 0));

	// Unpack the specular power from the alpha component of the specular color.
	float specularPower = exp2(specular.a * 10.5f);

	Light light = L;//Lights[LightIndex];

	Material mat = (Material)0;
	mat.Diffuse = diffuse;
	mat.Specular = specular;
	mat.SpecularFactor = specularPower;

	LightingResult lit = (LightingResult)0;

	switch (light.Type)
	{
		case DIRECTIONAL_LIGHT:
		{
			lit = DoDirectionalLight(light, mat, V, P, N);
		}
		break;
		case POINT_LIGHT:
		{
			lit = DoPointLight(light, mat, V, P, N);
		}
		break;
		case SPOT_LIGHT:
		{
			lit = DoSpotLight(light, mat, V, P, N);
		}
		break;
	}

	float4 colorResult = (diffuse * lit.Diffuse) + (specular * lit.Specular);
	
	const float bias = 0.00001f;

	const float4x4 mvpl = mul(LightProjectionMatrix, LightViewMatrix);
	float4 lightViewPosition = mul(mvpl, nativePosition);

	float2 projectTexCoord = (float2)0;
	projectTexCoord.x = (lightViewPosition.x / lightViewPosition.w) * 0.5f + 0.5f; // From (-1; 1) to (0-1)
	projectTexCoord.y = (-lightViewPosition.y / lightViewPosition.w) * 0.5f + 0.5f;

	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		float depthValue = Blur(TextureShadow, LinearClampSampler, projectTexCoord);
		//float depthValue = TextureShadow.Sample(LinearClampSampler, projectTexCoord).r;

		float lightDepthValue = (lightViewPosition.z / lightViewPosition.w);
		lightDepthValue -= bias;

		if (lightDepthValue < depthValue)
		{
			//colorResult = ;
		}
		else
		{
			colorResult *= 0.1f;
		}
	}

	return colorResult;
}