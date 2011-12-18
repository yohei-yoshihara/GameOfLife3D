#define NUMBER_OF_LIGHTS 2

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vLightDir[NUMBER_OF_LIGHTS];
	float4 vLightColor[NUMBER_OF_LIGHTS];
}


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos   : POSITION0;
    float3 Norm  : NORMAL0;
    float4 Color : COLOR0;
};

struct PS_INPUT
{
    float4 Pos   : SV_POSITION;
    float3 Norm  : TEXCOORD0;
    float4 Color : COLOR0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0.0f;
    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Norm = mul( float4(input.Norm, 1.0), World ).xyz;
    output.Color = input.Color;
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input ) : SV_Target0
{
    float4 finalColor = 0.0f;
    
    for(int i = 0; i < NUMBER_OF_LIGHTS; ++i) {
        finalColor += saturate( 
			dot((float3)vLightDir[i], input.Norm) 
			* vLightColor[i] 
			* input.Color);
    }
    finalColor.a = 1.0f;
    return finalColor;
}
