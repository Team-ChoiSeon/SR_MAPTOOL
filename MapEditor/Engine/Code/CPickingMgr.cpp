#include "Engine_Define.h"
#include "CPickingMgr.h"
#include "CGraphicDev.h"
#include "CInputMgr.h"
#include "CSceneMgr.h"
#include "GUISystem.h"
#include "CCameraMgr.h"
#include "CEditorSystem.h"

#include "CScene.h"
#include "CLayer.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CCamera.h"
#include "CModel.h"
#include "CMesh.h"


IMPLEMENT_SINGLETON(CPickingMgr)

CPickingMgr::CPickingMgr()
	:m_pDevice(nullptr), m_RayHitted{}, m_Picked{}
{
}

CPickingMgr::~CPickingMgr()
{
	Free();
}

HRESULT CPickingMgr::Ready_Picking(HWND g_hWnd)
{
	m_pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();
	if (!m_pDevice) {
		return E_FAIL;
	}
	m_pDevice->AddRef();
	m_hwnd = g_hWnd;
	return S_OK;
}

void CPickingMgr::Update_Picking(_float& dt)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hwnd, &pt);
	m_CursorPos = pt;

	//m_RayHitted = { nullptr, 0.f };
	//m_Picked = { nullptr, 0.f };
	ComputeRay();
}

void CPickingMgr::LateUpdate_Picking(_float& dt)
{
	Check_Input();
}

void CPickingMgr::Register_Info(RAYHIT info)
{
	if (!info.target || !CSceneMgr::GetInstance()->Is_ValidObject(info.target))
		return; // 무효 대상은 피킹 등록하지 않음

	m_PickTarget.push_back(info);
}

void CPickingMgr::Remove_Info(CGameObject* obj)
{
	auto iter = find_if(m_PickTarget.begin(), m_PickTarget.end(), [&obj](RAYHIT data) ->bool{
		return obj == data.target;
		});

	if (iter != m_PickTarget.end())
		m_PickTarget.erase(iter);
}

void CPickingMgr::ComputeRay()
{
	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9)); //dx 뷰포트 가져오기
	m_pDevice->GetViewport(&ViewPort);

	_vec3 ndc;
	ndc.x = ((float)m_CursorPos.x / (float)ViewPort.Width) * 2.0f - 1.0f;
	ndc.y = 1.0f - ((float)m_CursorPos.y / (float)ViewPort.Height) * 2.0f;
	ndc.z = 1.0f;

	// 투영 -> 뷰 스페이스
	D3DXMATRIX	matProj;
	matProj = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ProjMatrix();
	D3DXMatrixInverse(&matProj, 0, &matProj); //역행렬 돌려줌.
	D3DXVec3TransformCoord(&ndc, &ndc, &matProj); //그 역행렬을 기준으로 ndc를 계산함.

	// 뷰 스페이스 -> 월드
	D3DXMATRIX	matView;
	matView = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ViewMatrix();
	D3DXMatrixInverse(&matView, 0, &matView);

	m_tRay.vRayPos = { 0.f, 0.f, 0.f };
	m_tRay.vRayDir = ndc - m_tRay.vRayPos; 

	D3DXVec3TransformCoord(&m_tRay.vRayPos, &m_tRay.vRayPos, &matView); //레이포즈를 월드 위치 벡터로 변환
	D3DXVec3TransformNormal(&m_tRay.vRayDir, &m_tRay.vRayDir, &matView); //레이 디렉션을 월드 방향 벡터로 변환
	D3DXVec3Normalize(&m_tRay.vRayDir, &m_tRay.vRayDir);
}

void CPickingMgr::SortRayHit()
{
	if (m_PickTarget.empty()) {
		m_RayHitted = { nullptr,0 };
		return;
	}

	//2. Check Nearest Object
	auto iter = min_element(m_PickTarget.begin(), m_PickTarget.end(),
		[](const RAYHIT& a, const RAYHIT& b) {
			
			return a.fDistance < b.fDistance;
		});

	m_RayHitted = *iter;
	m_PickTarget.clear();
}

void CPickingMgr::Free()
{
	Safe_Release(m_pDevice);
}

void CPickingMgr::Check_Input()
{
	if (GUISystem::GetInstance()->UsingUI()) return;
	if (CInputMgr::GetInstance()->Mouse_Tap(DIM_LB))
	{
		if (m_PickTarget.empty()) {
			m_Picked = { nullptr, 0.f };
			m_RayHitted = { nullptr, 0.f };
			CSceneMgr::GetInstance()->Set_SelectedObject(nullptr); // ← 명시적으로 해제
			return;
		}

		SortRayHit();
		if (m_RayHitted.target && CSceneMgr::GetInstance()->Is_ValidObject(m_RayHitted.target))
		{
			m_Picked = m_RayHitted;
			CSceneMgr::GetInstance()->Set_SelectedObject(m_Picked.target);
		}
		else
		{
			m_Picked = { nullptr, 0.f };
			CSceneMgr::GetInstance()->Set_SelectedObject(nullptr);
		}

	}
}

CGameObject* CPickingMgr::Get_HittedObject() const
{
	return m_RayHitted.target;
}

void CPickingMgr::Clear_AllPickData()
{
	m_RayHitted = { nullptr, 0.f };
	m_Picked = { nullptr, 0.f };
	m_PickTarget.clear();
}