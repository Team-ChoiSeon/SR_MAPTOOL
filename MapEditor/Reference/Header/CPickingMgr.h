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
	void ComputeRay(); //���� ���� ���
	RAY RayToLocal(CTransform* transform); //���� ���� ���
	void Start_RayCasting(); //������ ������Ʈ ��ȸ
	_float Calc_ObjRay(CGameObject* obj); //������Ʈ ���� �浹 (AABB)

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