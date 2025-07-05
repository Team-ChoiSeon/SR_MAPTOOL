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
    void Set_Emissive(CTexture* tex);
    void Set_Specular(CTexture* tex);
    void Set_Shader(const string& path); // ShaderMgr 연동

    CTexture* Get_Diffuse() const { return m_pDiffuse; }
    CTexture* Get_Normal() const { return m_pNormal; }
    CTexture* Get_Roughness() const { return m_pRoughness; }
    CTexture* Get_Emissive() const { return m_pEmissive; }
    CTexture* Get_Specular() const { return m_pSpecular; }
    LPD3DXEFFECT Get_Effect() const { return m_pEffect; }

    string& Get_MaterialKey() { return m_strKey; }
    void Set_MatrialKey(const string& key) {  m_strKey = key; }

    string& Get_ShaderKey() { return m_strShaderPath; }
    void Apply(LPDIRECT3DDEVICE9 pDevice); // GPU 바인딩

private:
    HRESULT Ready_Material();

private:
    CTexture* m_pDiffuse = nullptr;
    CTexture* m_pNormal = nullptr;
    CTexture* m_pRoughness = nullptr;
    CTexture* m_pEmissive = nullptr;
    CTexture* m_pSpecular = nullptr;
    string m_strKey; // 머티리얼 키

    LPD3DXEFFECT m_pEffect = nullptr; // 셰이더 객체
    string m_strShaderPath; //셰이더 키

private:
    void Free() override;
};
END
