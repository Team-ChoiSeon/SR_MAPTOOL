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
    void Apply(LPDIRECT3DDEVICE9 pDevice); // GPU ¹ÙÀÎµù

private:
    HRESULT Ready_Material();

private:
    CTexture* m_pDiffuse = nullptr;
    CTexture* m_pNormal = nullptr;
    CTexture* m_pRoughness = nullptr;

private:
    void Free() override;
};
END
