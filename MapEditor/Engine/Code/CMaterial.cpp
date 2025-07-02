#include "Engine_Define.h"
#include "CMaterial.h"
#include "CTexture.h"

CMaterial::CMaterial()
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
    m_pDiffuse = tex;
}

void CMaterial::Set_Normal(CTexture* tex)
{
    m_pNormal = tex;
}

void CMaterial::Set_Roughness(CTexture* tex)
{
    m_pRoughness = tex;
}

void CMaterial::Set_Emissive(CTexture* tex)
{
    m_pEmissive = tex;
}

void CMaterial::Set_Specular(CTexture* tex)
{
    m_pSpecular = tex;
}

void CMaterial::Apply(LPDIRECT3DDEVICE9 pDevice)
{
    if (m_pDiffuse)
        m_pDiffuse->Bind(pDevice, 0); // stage 0
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
}
