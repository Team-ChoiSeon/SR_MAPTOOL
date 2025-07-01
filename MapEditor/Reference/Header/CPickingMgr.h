#pragma once
#include "CBase.h"

BEGIN(Engine)
class CTransform;
class CGameObject;

class ENGINE_DLL CPickingMgr :
	public CBase
{
	DECLARE_SINGLETON(CPickingMgr);

	enum class Screen_Mode {MoveCam, RotateCam, MoveObj, RotateObj, ResizeObj };
private:
	explicit CPickingMgr();
	virtual ~CPickingMgr();

public:
	HRESULT Ready_Picking(HWND g_hWnd);
	void Update_Picking(_float& dt);
	void LateUpdate_Picking(_float& dt);

public:
	CGameObject* Get_PickedObj() { return m_pTarget; }
	void Set_PickedObj(CGameObject* target) { m_pTarget = target; }

private:
	void ComputeRay(); //월드 광선 계산
	RAY RayToLocal(CTransform* transform); //로컬 광선 계산
	void Start_RayCasting(); //광선과 오브젝트 순회
	_float Calc_ObjRay(CGameObject* obj); //오브젝트 광선 충돌 (AABB)

private:

private:
	HWND m_hwnd;
	POINT m_CursorPos;
	LPDIRECT3DDEVICE9 m_pDevice;
	RAY m_tRay;
	Screen_Mode m_eMode;
	CGameObject* m_pTarget;
private:
	void Free();
};

END