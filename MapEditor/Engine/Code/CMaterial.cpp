#include "Engine_Define.h"
#include "CMaterial.h"
#include "CTexture.h"
#include "CShaderMgr.h"


CMaterial::CMaterial()
	: m_pDiffuse(nullptr)
	, m_pNormal(nullptr)
	, m_pRoughness(nullptr)
	, m_pEmissive(nullptr)
	, m_pSpecular(nullptr)
{
}

CMaterial::~CMaterial()
{
}

CMaterial* CMaterial::Create()
{
	CMaterial* instance = new CMaterial;

	if (FAILED(instance->Ready_Material())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

void CMaterial::Set_Diffuse(CTexture* tex)
{
	if (m_pDiffuse)
		return;

	m_pDiffuse = tex;

	if (m_pDiffuse)
		m_pDiffuse->AddRef();
}

void CMaterial::Set_Normal(CTexture* tex)
{
	if (m_pNormal)
		m_pNormal->Release();

	m_pNormal = tex;

	if (m_pNormal)
		m_pNormal->AddRef();
}

void CMaterial::Set_Roughness(CTexture* tex)
{
	if (m_pRoughness)
		m_pRoughness->Release();

	m_pRoughness = tex;

	if (m_pRoughness)
		m_pRoughness->AddRef();
}

void CMaterial::Set_Emissive(CTexture* tex)
{
	if (m_pEmissive)
		m_pEmissive->Release();

	m_pEmissive = tex;

	if (m_pEmissive)
		m_pEmissive->AddRef();
}

void CMaterial::Set_Specular(CTexture* tex)
{
	if (m_pSpecular)
		m_pSpecular->Release();

	m_pSpecular = tex;

	if (m_pSpecular)
		m_pSpecular->AddRef();
}

void CMaterial::Set_Shader(const string& path)
{
	m_pEffect = CShaderMgr::GetInstance()->GetShader(path);
	if (m_pEffect)
		m_strShaderPath = path;
	else
		m_strShaderPath.clear(); // 로딩 실패 시 키 제거
}


void CMaterial::Apply(LPDIRECT3DDEVICE9 pDevice)
{
	if (m_pEffect)
	{


		// 셰이더 파라미터 바인딩
		if (m_pDiffuse)  m_pEffect->SetTexture("g_DiffuseTex", m_pDiffuse->Get_Handle());
		if (m_pNormal)   m_pEffect->SetTexture("g_NormalTex", m_pNormal->Get_Handle());
		if (m_pRoughness)m_pEffect->SetTexture("g_RoughnessTex", m_pRoughness->Get_Handle());
		if (m_pEmissive) m_pEffect->SetTexture("g_EmissiveTex", m_pEmissive->Get_Handle());
		if (m_pSpecular) m_pEffect->SetTexture("g_SpecularTex", m_pSpecular->Get_Handle());
		//  추가: 기본 UVScale 지정 (디버깅 목적으로라도)

		return;
	}

	// 셰이더가 없을 때: 고정 파이프라인 설정 (Fixed Function)
	if (m_pDiffuse)
	{
		m_pDiffuse->Bind(pDevice, 0);
	}
}


HRESULT CMaterial::Ready_Material()
{
	return S_OK;
}

void CMaterial::Free()
{
	Safe_Release(m_pDiffuse);
	Safe_Release(m_pNormal);
	Safe_Release(m_pRoughness);
	Safe_Release(m_pEmissive);
	Safe_Release(m_pSpecular);
}

