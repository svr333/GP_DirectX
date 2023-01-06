//-----------------------------------------------------
//  Global Variables
//-----------------------------------------------------
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

//-----------------------------------------------------
//  Input/Output Structs
//-----------------------------------------------------
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2 Uv: TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
    float2 Uv: TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

//-----------------------------------------------------
//  Vertex Shader
//-----------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = float4(input.Position, 1.0f);
    output.Position = mul(output.Position, gWorldViewProj);
    output.Color = input.Color;
    output.Uv = input.Uv;
    output.Normal = input.Normal;
    output.Tangent = input.Tangent;
    return output;
}

//-----------------------------------------------------
//  Pixel Shader
//-----------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float3 color = gDiffuseMap.Sample(samPoint, input.Uv) * input.Color;
    return float4(color, 1.f);
}

//-----------------------------------------------------
//  Technique
//-----------------------------------------------------
technique11 DefaultTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
