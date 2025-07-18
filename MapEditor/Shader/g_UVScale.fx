// ===== 상수 정의 =====
float4x4 g_matWorld;
float4x4 g_matView;
float4x4 g_matProj;

texture g_DiffuseTex;
texture g_NormalTex;

bool g_usingLight;
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
    float3 T = normalize(mul(input.tangent, (float3x3) g_matWorld));    //탄젠트 곱 월드
    float3 B = normalize(mul(input.binormal, (float3x3) g_matWorld));  //바이노말 곱 월드
    float3 N = normalize(mul(input.normal, (float3x3) g_matWorld));     //노말 곱 월드
    //여기까지는 기존 다렉 머티리얼과 같은 방식
    //그러나 정점의 TBN을 구해줘야, 그 면의 요철(픽셀 노멀)을 적절히 적용할 수 있기 때문
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
    //Tangent Space란 정점 기준의 로컬
    float3 normalTS = tex2D(NormalSampler, input.uv).rgb * 2.0f - 1.0f; // Tangent Space normal
    //색은 0~1까지로 표현되기 때문에 -1~1 사이의 공간으로 변환 시킨 과정임
    normalTS = normalize(normalTS); //정규화

    // TBN → 월드 노멀
    float3 normalWS = normalize(mul(normalTS, input.TBN)); // 위에서 구한 버텍스 TBN과 노말맵의 탄젠트 곱
    // m_vRight = _vec3(m_WorldMat._11, m_WorldMat._12, m_WorldMat._13);
    // m_vUp = _vec3(m_WorldMat._21, m_WorldMat._22, m_WorldMat._23);
    // m_vLook = _vec3(m_WorldMat._31, m_WorldMat._32, m_WorldMat._33);
    //를 생각해보면, 33은 결국 각자의 축을 표현하는 벡터의 합임. 그렇기에 각 정점 기준 로컬 좌표인 TBN을 -> 축 벡터와 동일시하여
    //노멀맵의 벡터(r,g,b)를 공간 변환해주는 과정임
    
   // float3 lightDirWS = normalize(-g_LightDir); // 광원이 향하는 방향 -> 이건 이미 내가 음수 곱 해서 보내줬음.
    
    //내적으로 각도를 계산(단위 벡터니께), 그 각도의 크기에 따라 0~1사이로 클램핑->색상으로 다시 변환.
    float diff = saturate(dot(normalWS, g_LightDir));

    float3 baseColor = tex2D(DiffuseSampler, input.uv).rgb;

    //(조명 색 + ambient) * 텍스처 색
    float3 litColor = g_Ambient.rgb + g_LightColor.rgb * diff;
    if (g_usingLight)
    {
        return float4(baseColor, 1.f);
    }
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
