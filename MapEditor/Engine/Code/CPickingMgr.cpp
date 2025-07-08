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


	ComputeRay();
	//m_PickTarget.clear();  
}

void CPickingMgr::LateUpdate_Picking(_float& dt)
{
	Check_Input();
}

void CPickingMgr::Register_Info(RAYHIT info)
{
	m_PickTarget.push_back(info);
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
		[](const RAYHIT& a, const RAYHIT& b) {return a.fDistance < b.fDistance;
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
		SortRayHit();

		m_Picked = m_RayHitted;
		CEditorSystem::GetInstance()->Set_Select(m_Picked.target);
		CSceneMgr::GetInstance()->Set_SelectedObject(m_Picked.target);
	}
}

