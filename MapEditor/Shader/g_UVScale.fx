// ===== 상수 정의 =====
float4x4 g_matWorldViewProj;
texture g_DiffuseTex;
float4 g_UVScale = float4(1.0f, 1.0f, 0.0f, 0.0f); // xy만 사용
float4 g_UVPosition = float4(1.0f, 1.0f, 0.0f, 0.0f); // xy만 사용

// ===== 구조체 정의 =====
struct VS_IN {
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct VS_OUT {
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

// ===== 정점 셰이더 =====
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;
    output.pos = mul(input.pos, g_matWorldViewProj);
    output.uv = input.uv * g_UVScale.xy;
    output.uv = input.uv + g_UVPosition.xy;
    return output;
}

// ===== 샘플러 =====
sampler DiffuseSampler = sampler_state
{
    Texture = <g_DiffuseTex>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};

// ===== 픽셀 셰이더 =====
float4 PS_Main(VS_OUT input) : COLOR0
{
    return tex2D(DiffuseSampler, input.uv);
}

// ===== 기법 =====
technique Default
{
    pass P0
    {
        VertexShader = compile vs_2_0 VS_Main();
        PixelShader  = compile ps_2_0 PS_Main();
    }
}
