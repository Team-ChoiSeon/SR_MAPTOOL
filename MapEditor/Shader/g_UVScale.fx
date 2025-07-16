// ===== 상수 정의 =====
float4x4 g_matWorld;
float4x4 g_matView;
float4x4 g_matProj;

texture g_DiffuseTex;
texture g_NormalTex;

float3 g_LightDir;
float4 g_LightColor;
float4 g_Ambient;

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

    float4x4 g_matWorldViewProj = mul(mul(g_matWorld, g_matView), g_matProj);
    output.pos = mul(input.pos, g_matWorldViewProj);
    output.uv = input.uv * g_UVScale.xy;//래핑 용도

    //정점의 법선을 월드로
    float3 T = normalize(mul(input.tangent, (float3x3) g_matWorld)); //탄젠트 곱 월드
    float3 B = normalize(mul(input.binormal, (float3x3) g_matWorld)); //바이노말 곱 월드
    float3 N = normalize(mul(input.normal, (float3x3) g_matWorld)); //노말 곱 월드

    output.TBN = float3x3(T, B, N);

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
    //노말 텍스처 색상 가져오기
    float3 normalTS = tex2D(NormalSampler, input.uv).rgb * 2.0f - 1.0f; // Tangent Space normal
    normalTS = normalize(normalTS);

    // TBN → 월드 노멀
    float3 normalWS = normalize(mul(normalTS, input.TBN)); // TBN * normalTS
    float3 lightDirWS = normalize(-g_LightDir); // 광원이 향하는 방향

    //0~1사이 제한
    float diff = saturate(dot(normalWS, lightDirWS));

    float3 baseColor = tex2D(DiffuseSampler, input.uv).rgb;

    float3 litColor = g_Ambient.rgb + g_LightColor.rgb * diff;
    return float4(baseColor * litColor, 1.0f);
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
