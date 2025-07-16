#pragma once
#include "CBase.h"

BEGIN(Engine)

class CLight;

class ENGINE_DLL CLightMgr:
    public CBase
{
    DECLARE_SINGLETON(CLightMgr)

private:
    explicit  CLightMgr();
    virtual ~CLightMgr();

public:
    HRESULT Ready_Light(LPDIRECT3DDEVICE9 pDevice);
    void Update_Light(_float& dt);
    void LateUpdate_Light(_float& dt);
    void Add_Light(CLight* light);
    void Remove_Light(CLight* light);
    void Render_Light();
    D3DLIGHT9& Get_MainLight() { return m_BaseLight; }
private:
    void Sort_Light();
    float DistanceSqare(_vec3 light, _vec3 cam);

private:
    const int m_MaxSize = 8;
    int m_curMaxSize ;
    vector<CLight*> m_LightContainer;
    LPDIRECT3DDEVICE9 m_pDevice;
    D3DLIGHT9 m_BaseLight;
private:
    void Free();
};

END