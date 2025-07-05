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
    CCamera* Get_MainCamera();
private:
    void Move_DeafultCam(_float& dt);
private:
    LPDIRECT3DDEVICE9 m_pDevice;
    CCamera* m_pMainCam;
    CGameObject* m_pDefaultCam = nullptr; // 기본 카메라
private:
    void Free();
};

END
