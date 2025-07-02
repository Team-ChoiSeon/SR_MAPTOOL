#pragma once
#include "CBase.h"
BEGIN(Engine)

class CTexture;

class ENGINE_DLL CMaterial :
    public CBase
{
private:
   explicit CMaterial();
   virtual ~CMaterial();
public:
    static CMaterial* Create();
public:
    void Set_Diffuse(CTexture* tex);
    void Set_Normal(CTexture* tex);
    void Set_Roughness(CTexture* tex);

    CTexture* Get_Diffuse() const { return m_pDiffuse; }
    CTexture* Get_Normal() const { return m_pNormal; }
    CTexture* Get_Roughness() const { return m_pRoughness; }
    CTexture* Get_Emissive() const { return m_pEmissive; }
    CTexture* Get_Specular() const { return m_pSpecular; }

    string& Get_Key() { return m_strKey; }
    void Apply(LPDIRECT3DDEVICE9 pDevice); // GPU ¹ÙÀÎµù

private:
    HRESULT Ready_Material();

private:
    CTexture* m_pDiffuse = nullptr;
    CTexture* m_pNormal = nullptr;
    CTexture* m_pRoughness = nullptr;
    CTexture* m_pEmissive = nullptr;
    CTexture* m_pSpecular = nullptr;


    std::string m_strKey;
private:
    void Free() override;
};
END
