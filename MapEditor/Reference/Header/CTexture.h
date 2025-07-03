#pragma once
#include "CBase.h"

BEGIN(Engine)
class ENGINE_DLL CTexture :
    public CBase
{
private:
    explicit CTexture();
    virtual ~CTexture();
public:
    static CTexture* Create();
public:
    HRESULT Load(LPDIRECT3DDEVICE9 pDevice, const std::string& filePath); // ��� ��� �ε�
    void Bind(LPDIRECT3DDEVICE9 pDevice,_uint stage = 0);                // �ؽ�ó ���ε�

    const std::string& GetKey() const { return m_strKey; }
    void SetKey(const string& key) { m_strKey = key; }

private:
    HRESULT Ready_Texture();

private:
    LPDIRECT3DTEXTURE9 m_pTexture = nullptr;
    std::string m_strKey;
private:
    void Free() override;
};

END