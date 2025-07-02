#pragma once
#include "CBase.h"

BEGIN(Engine)
class CTransform;
class CGameObject;

class ENGINE_DLL CPickingMgr :
	public CBase
{
	DECLARE_SINGLETON(CPickingMgr);

	enum class Screen_Mode {CamMode, ObjectMode };
	enum class Move_Mode {Transform, Scale, Rotate };
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

private://��ŷ ��� ����
	void Mode_Check(_float& dt);
	void Key_Check(_float& dt);
	void Mode_Panel();
	void MoveCamera(_float& dt);
	void MoveObject(_float& dt);

private:
	HWND m_hwnd;
	POINT m_CursorPos;
	LPDIRECT3DDEVICE9 m_pDevice;
	RAY m_tRay;
	Screen_Mode m_eMode;
	Move_Mode m_eMove;
	CGameObject* m_pTarget;

private:
	void Free();
};

END