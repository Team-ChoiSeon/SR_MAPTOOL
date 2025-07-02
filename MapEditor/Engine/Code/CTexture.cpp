#include "Engine_Define.h"
#include "CTexture.h"

CTexture::CTexture()
{
}

CTexture::~CTexture()
{
}

CTexture* CTexture::Create()
{
    CTexture* instance = new CTexture;
    
    if (FAILED(instance->Ready_Texture())) {
        Safe_Release(instance);
        instance = nullptr;
    }

    return instance;
}

HRESULT CTexture::Load(LPDIRECT3DDEVICE9 pDevice, const std::string& filePath)
{
    if (!pDevice) return E_FAIL;

    if (FAILED(D3DXCreateTextureFromFileA(pDevice, filePath.c_str(), &m_pTexture)))
    {
        MessageBoxA(0, ("Failed to load texture: " + filePath).c_str(), "Error", MB_OK);
        return E_FAIL;
    }

    m_strKey = filePath;
    return S_OK;
}

void CTexture::Bind(LPDIRECT3DDEVICE9 pDevice, _uint stage)
{
    if (m_pTexture && pDevice)
        pDevice->SetTexture(stage, m_pTexture);
}

HRESULT CTexture::Ready_Texture()
{
    return S_OK;
}

void CTexture::Free()
{
    if (m_pTexture)
    {
        Safe_Release(m_pTexture);
    }
}
