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
    if (!pDevice || filePath.empty())
        return E_FAIL;

    HRESULT hr = D3DXCreateTextureFromFileExA(
        pDevice,
        filePath.c_str(),
        D3DX_DEFAULT, D3DX_DEFAULT,     // 원본 사이즈
        D3DX_DEFAULT,                   // MipLevel
        0,                              // Usage
        D3DFMT_A8R8G8B8,                // Force alpha-aware format
        D3DPOOL_MANAGED,                // 일반적인 풀
        D3DX_DEFAULT, D3DX_DEFAULT,     // 필터
        0,                              // ColorKey (0: 없음)
        nullptr, nullptr,
        &m_pTexture
    );

    if (FAILED(hr))
    {
        MessageBoxA(0, ("Failed to load texture: " + filePath).c_str(), "Error", MB_OK);
        return E_FAIL;
    }

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
