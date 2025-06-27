#pragma once
#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer :
    public CBase
{
    DECLARE_SINGLETON(CRenderer)

private:
    explicit CRenderer();
    virtual ~CRenderer();

public: 
    HRESULT Ready_Renderer();
    void Render();

private:
    LPDIRECT3DDEVICE9 m_pDevice;

private:
    void Free() override;

};

END