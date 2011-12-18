#define NUMBER_OF_LIGHTS 2

// Cube vertices
static const float3 s_vertices[36] = {
	{-1.0f, 1.0f,  1.0f}, // 3
	{ 1.0f, 1.0f, -1.0f}, // 1
	{-1.0f, 1.0f, -1.0f}, // 0
	{ 1.0f, 1.0f,  1.0f}, // 2
	{ 1.0f, 1.0f, -1.0f}, // 1
	{-1.0f, 1.0f,  1.0f}, // 3

	{ 1.0f, -1.0f,  1.0f}, // 6
	{-1.0f, -1.0f, -1.0f}, // 4
	{ 1.0f, -1.0f, -1.0f}, // 5
	{-1.0f, -1.0f,  1.0f}, // 7
	{-1.0f, -1.0f, -1.0f}, // 4
	{ 1.0f, -1.0f,  1.0f}, // 6

	{-1.0f,  1.0f,  1.0f}, // 11
	{-1.0f, -1.0f, -1.0f}, // 9
	{-1.0f, -1.0f,  1.0f}, // 8
	{-1.0f,  1.0f, -1.0f}, // 10
	{-1.0f, -1.0f, -1.0f}, // 9
	{-1.0f,  1.0f,  1.0f}, // 11

	{1.0f,  1.0f, -1.0f}, // 14
	{1.0f, -1.0f,  1.0f}, // 12
	{1.0f, -1.0f, -1.0f}, // 13
	{1.0f,  1.0f,  1.0f}, // 15
	{1.0f, -1.0f,  1.0f}, // 12
	{1.0f,  1.0f, -1.0f}, // 14

	{-1.0f,  1.0f, -1.0f}, // 19
	{ 1.0f, -1.0f, -1.0f}, // 17
	{-1.0f, -1.0f, -1.0f}, // 16
	{ 1.0f,  1.0f, -1.0f}, // 18
	{ 1.0f, -1.0f, -1.0f}, // 17
	{-1.0f,  1.0f, -1.0f}, // 19

	{ 1.0f,  1.0f, 1.0f}, // 22
	{-1.0f, -1.0f, 1.0f}, // 20
	{ 1.0f, -1.0f, 1.0f}, // 21
	{-1.0f,  1.0f, 1.0f}, // 23
	{-1.0f, -1.0f, 1.0f}, // 20
	{ 1.0f,  1.0f, 1.0f}, // 22
};

// Cube normals
static const float3 s_normals[6] = {
	{0.0f, 1.0f, 0.0f},
	{0.0f, -1.0f, 0.0f},
	{-1.0f, 0.0f, 0.0f},
	{1.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, -1.0f},
	{0.0f, 0.0f, 1.0f},
};

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vLightDir[NUMBER_OF_LIGHTS];
	float4 vLightColor[NUMBER_OF_LIGHTS];
}

cbuffer CBParams : register( b1 )
{
	int width;
	int height;
	int width_x_height; // = width * height
	int generations;
	float3 p0;
	float3 cubeLength;
	float3 halfCubeLength; // = cubeLength / 2.0f
	float3 cubeMargin;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Color : COLOR0; // xyz -> color, w -> alive
	uint   Id    : SV_VERTEXID;
};

struct VS_OUTPUT // GS_INPUT
{
	float3 Pos   : POSITION0; // cube center position
	float4 Color : COLOR0;
};

struct GS_OUTPUT
{
    float4 Pos   : SV_POSITION;
    float3 Norm  : NORMAL0;
    float4 Color : COLOR0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( VS_INPUT input )
{
	int g = input.Id / width_x_height;
	int temp = input.Id - g * width_x_height;
	int y = temp / width;
	int x = temp % width;

	VS_OUTPUT output;
    output.Pos.x = p0.x + (cubeLength.x + cubeMargin.x) * x + halfCubeLength.x;
    output.Pos.y = p0.y + (cubeLength.y + cubeMargin.y) * y + halfCubeLength.y;
    output.Pos.z = p0.z + (cubeLength.z + cubeMargin.z) * g + halfCubeLength.z;
	
	if(input.Color.w > 0.01f) {
		float colorRate = ((float)g) / generations;
		output.Color = float4(
			input.Color.x * colorRate, 
			input.Color.y * colorRate, 
			input.Color.z * colorRate, 
			input.Color.w);
	}
	else {
		output.Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
    return output;
}

//--------------------------------------------------------------------------------------
// Geometry Shader
//--------------------------------------------------------------------------------------
[maxvertexcount(36)]
void GS(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> cubeStream)
{
	GS_OUTPUT output;
	
	if(input[0].Color.w > 0.01f) {
		for(int i = 0; i < 6; ++i) {
			float3 normal = s_normals[i];
			for(int j = 0; j < 2; ++j) {
				for(int k = 0; k < 3; ++k) {
					float4 pos = float4(input[0].Pos + s_vertices[i * 6 + j * 3 + k] * halfCubeLength, 1.0f);		
					output.Pos = mul( pos, World );
					output.Pos = mul( output.Pos, View );
					output.Pos = mul( output.Pos, Projection );
					output.Norm = mul( float4(normal, 1.0), World ).xyz;
					output.Color = input[0].Color;
					cubeStream.Append(output);
				}
				cubeStream.RestartStrip();
			}
		}
	}
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( GS_OUTPUT input ) : SV_Target0
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
