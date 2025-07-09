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
    _bool Is_DefaultCam() { return m_bDefault; };
    _bool Is_Moving() { return IsCameraInputActive(); };

private:
    void Move_DeafultCam(_float& dt);
    _bool IsCameraInputActive() const;

private:
    LPDIRECT3DDEVICE9 m_pDevice;
    CCamera* m_pMainCam;
    CGameObject* m_pDefaultCam = nullptr; // 기본 카메라"
    _bool m_bDefault;
private:
    void Free();
};

END
