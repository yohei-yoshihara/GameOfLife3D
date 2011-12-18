struct LightInfo
{
    float3 pos;
    float3 dir;
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float3 attenuate;
};

struct SurfaceInfo
{
    float3 pos;
    float3 normal;
    float4 diffuse;
    float4 specular;
};

cbuffer CBPerFrame : register( b0 )
{
    LightInfo c_Light;
    float3 c_EyePosW;
};

cbuffer CBPerObject : register( b1 )
{
    float4x4 c_World;
    float4x4 c_WVP; // world x view x projection
    float4x4 c_TexMtx;
};

Texture2D g_DiffuseMap;
Texture2D g_SpecularMap;

SamplerState g_TriLinearSample
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_IN
{
    float3 posL    : POSITION;
    float3 normalL : NORMAL;
    float2 texC    : TEXCOORD;
};

struct VS_OUT
{
    float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float2 texC0   : TEXCOORD0;
    float2 texC1   : TEXCOORD1;
};

VS_OUT VS(VS_IN vIn)
{
    VS_OUT vOut;
    vOut.posW = mul(float4(vIn.posL, 1.0f), c_World).xyz;
    vOut.normalW = mul(float4(vIn.normalL, 1.0f), c_World).xyz;
    vOut.posH = mul(float4(vIn.posL, 1.0f), c_WVP);
    vOut.texC0 = vIn.texC;
    vOut.texC1 = mul(float4(vIn.texC, 0.0f, 1.0f), c_TexMtx).xy;
    return vOut;
}

float4 PS(VS_OUT pIn) : SV_TARGET0
{
    float alpha     = g_DiffuseMap.Sample(g_TriLinearSample, pIn.texC0).a;
    float4 diffuse  = g_DiffuseMap.Sample(g_TriLinearSample, pIn.texC1);
    float4 specular = g_SpecularMap.Sample(g_TriLinearSample, pIn.texC1);
    
    specular.a *= 256.0f;
    
    float3 normalW = normalize(pIn.normalW);

    SurfaceInfo v = {pIn.posW, normalW, diffuse, specular};
    
    float3 litColor = float3(0.0f, 0.0f, 0.0f);
    float3 lightVec = -c_Light.dir;
    litColor += (v.diffuse * c_Light.ambient).xyz;
    float diffuseFactor = dot(lightVec, v.normal);
    
    if(diffuseFactor > 0.0f) {
        float specularPower = max(v.specular.a, 1.0f);
        float3 toEye = normalize(c_EyePosW - v.pos);
        float3 R = reflect(-lightVec, v.normal);
        float specularFactor = pow(max(dot(R, toEye), 0.0f), specularPower);
        
        litColor += (diffuseFactor * v.diffuse * c_Light.diffuse).xyz;
        litColor += (specularFactor * v.specular * c_Light.specular).xyz;
    }
    return float4(litColor, alpha);
}

technique10 Default3D
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}

