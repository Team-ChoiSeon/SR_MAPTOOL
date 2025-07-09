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
	CGameObject* Get_PickedObj() { return m_Picked.target; }
	void Set_PickedObj(CGameObject* target) { m_Picked.target = target; }
	RAY Get_Ray() { return m_tRay; }
	void Register_Info(RAYHIT info);
	void Remove_Info(CGameObject* obj);
	CGameObject* Get_HittedObject() const;
	void Clear_AllPickData();

private:
	void ComputeRay(); //월드 광선 계산
	void SortRayHit(); //월드 광선 계산

private:
	HWND m_hwnd;
	POINT m_CursorPos;
	LPDIRECT3DDEVICE9 m_pDevice;
	RAY m_tRay;
	vector<RAYHIT> m_PickTarget;
	_vec3 m_vLastPt;
	_vec3 m_vPlanePt;
	_vec3 m_vPlaneNorm;
	_vec3 m_vDragDistance;

	RAYHIT			m_RayHitted;
	RAYHIT			m_Picked;

	_bool m_bDragging;
private:
	void Check_Input();
	void Free();
};

END