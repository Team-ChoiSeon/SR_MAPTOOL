#include "Engine_Define.h"
#include "CMaterial.h"
#include "CTexture.h"


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

void CMaterial::Apply(LPDIRECT3DDEVICE9 pDevice)
{
	if (m_pDiffuse)
		m_pDiffuse->Bind(pDevice, 0); // Stage 0 only for now
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

