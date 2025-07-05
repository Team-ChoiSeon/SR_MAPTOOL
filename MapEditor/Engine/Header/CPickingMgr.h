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
	RAY Get_Ray() { return m_tRay; }
	void Register_Info(RAYHIT info);

private:
	void ComputeRay(); //월드 광선 계산

private:
	HWND m_hwnd;
	POINT m_CursorPos;
	LPDIRECT3DDEVICE9 m_pDevice;
	RAY m_tRay;
	vector<RAYHIT> m_PickTarget;
	CGameObject* m_pTarget;

private:
	void Free();
};

END