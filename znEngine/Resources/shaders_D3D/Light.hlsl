#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTIONAL_LIGHT 2
#define UNKNOWN 3

struct Light
{
	// Position for point and spot lights (World space).
	float4 PositionWS;
	//--------------------------------------------------------------( 16 bytes )
	// Direction for spot and directional lights (World space).
	float4 DirectionWS;
	//--------------------------------------------------------------( 16 bytes )
	// Position for point and spot lights (View space).
	float4 PositionVS;
	//--------------------------------------------------------------( 16 bytes )
	// Direction for spot and directional lights (View space).
	float4 DirectionVS;
	//--------------------------------------------------------------( 16 bytes )
	// Color of the light. Diffuse and specular colors are not separated.
	float4 Color;
	//--------------------------------------------------------------( 16 bytes )
	// Disable or enable the light.
	uint Enabled;
	// The range of the light.
	float Range;
	// The intensity of the light.
	float Intensity;
	// The half angle of the spotlight cone.
	float SpotlightAngle;
	//--------------------------------------------------------------(16 bytes )
	// The type of the light.
	uint Type;
	float3 __Padding;
	//--------------------------------------------------------------(16 bytes )
	//--------------------------------------------------------------( 16 * 7 = 112 bytes )
};