#pragma once
#include "CBase.h"

BEGIN(Engine)
class CTransform;
class CGameObject;

class ENGINE_DLL CPickingMgr :
    public CBase
{
    DECLARE_SINGLETON(CPickingMgr);

private:
    explicit CPickingMgr();
    virtual ~CPickingMgr();

public:
    HRESULT Ready_Picking(HWND g_hWnd);
    void Update_Picking(_float& dt);
    void LateUpdate_Picking(_float& dt);
public:
    CGameObject* Get_PickedObj() { return m_pTarget; }
    void Set_PickedObj(CGameObject* target) {  m_pTarget = target; }
private:
    void ComputeRay();
    RAY RayToLocal(CTransform* transform);
    void Start_RayCasting();
    _float Calc_ObjRay(CGameObject* obj);
    
private:
    HWND m_hwnd;
    POINT m_CursorPos;
    LPDIRECT3DDEVICE9 m_pDevice;
    RAY m_tRay;
    CGameObject* m_pTarget;
private:
    void Free();
};

END