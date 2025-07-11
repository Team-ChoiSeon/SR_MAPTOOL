#pragma once
#include "CBase.h"
BEGIN(Engine)
class ENGINE_DLL CShaderMgr :
    public CBase
{
    DECLARE_SINGLETON(CShaderMgr)
private:
    explicit CShaderMgr();
    virtual ~CShaderMgr();

public:
    HRESULT Ready_Shader(LPDIRECT3DDEVICE9 pDevice);
    void Load_AllShaders(const string& folderPath);
    LPD3DXEFFECT GetShader(const string& path);
    HRESULT ReloadShader(const string& path); // �ַε�
    const vector<string>& Get_ShaderName() { return m_ShaderName; }
private:
    LPDIRECT3DDEVICE9 m_pDevice;
    unordered_map<string, LPD3DXEFFECT> m_mapShader;
    vector<string> m_ShaderName;
private:
    void Free() override;
};
END
