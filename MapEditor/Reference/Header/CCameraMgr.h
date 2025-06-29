#pragma once
#include "CBase.h"

BEGIN(Engine)

class CCamera;

class ENGINE_DLL CCameraMgr :
    public CBase
{
    DECLARE_SINGLETON(CCameraMgr)

private:
    explicit CCameraMgr();
    virtual ~CCameraMgr();

public:
    HRESULT Ready_CameraMgr();
    void Update_Camera(_float& dt);
    void LateUpdate_Camera(_float& dt);
public:
    void Set_MainCamera(CCamera* pCam);

private:
    LPDIRECT3DDEVICE9 m_pDevice;
    CCamera* m_pMainCam;

private:
    void Free();
};

END
