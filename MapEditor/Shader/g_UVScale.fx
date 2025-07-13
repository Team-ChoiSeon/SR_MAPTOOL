// ===== 상수 정의 =====
float4x4 g_matWorldViewProj;
float4x4 g_matWorld;

texture g_DiffuseTex;
texture g_NormalTex;

float4 g_UVScale = float4(1.0f, 1.0f, 0, 0); // xy만 사용

// ===== 정점 구조체 =====
struct VS_IN
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 tangent : TEXCOORD1;
    float3 binormal : TEXCOORD2;
};

struct VS_OUT
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
    float3x3 TBN : TEXCOORD1;
};

// ===== 정점 셰이더 =====
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;

    output.pos = mul(input.pos, g_matWorldViewProj);
    output.uv = input.uv * g_UVScale.xy;

    float3x3 worldTBN;
    worldTBN[0] = mul(input.tangent, (float3x3) g_matWorld);
    worldTBN[1] = mul(input.binormal, (float3x3) g_matWorld);
    worldTBN[2] = mul(input.normal, (float3x3) g_matWorld);

    output.TBN = worldTBN;
    return output;
}

// ===== 샘플러 정의 =====
sampler DiffuseSampler = sampler_state
{
    Texture = <g_DiffuseTex>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};

sampler NormalSampler = sampler_state
{
    Texture = <g_NormalTex>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};

// ===== 픽셀 셰이더 =====
float4 PS_Main(VS_OUT input) : COLOR0
{
    float4 baseColor = tex2D(DiffuseSampler, input.uv);
    return baseColor;
}

// ===== 기법 정의 =====
technique Default
{
    pass P0
    {
        VertexShader = compile vs_2_0 VS_Main();
        PixelShader = compile ps_2_0 PS_Main();
    }
}
