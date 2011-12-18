Texture2D tex : register( t0 );

SamplerState samp
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

struct InputVS
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct OutputVS
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

OutputVS VS(InputVS inVert)
{
	return inVert;
}

float4 PS(OutputVS inPixel) : SV_TARGET0
{
	return tex.Sample(samp, inPixel.tex);
}

technique10 Shader
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}
