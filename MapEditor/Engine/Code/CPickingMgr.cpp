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
	:m_pDevice(nullptr), m_pTarget(nullptr)
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
	ScreenToClient(m_hwnd, &pt);  // 스크린 → 클라이언트
	m_CursorPos = pt;

	if (GUISystem::GetInstance()->UsingUI()) return;

	if (CInputMgr::GetInstance()->Mouse_Down(DIM_RB))
		m_pTarget = nullptr;

	// 기즈모 조작 중이면 피킹 방지
	ComputeRay();

	if (CInputMgr::GetInstance()->Mouse_Down(DIM_LB)) {
		CEditorSystem::GetInstance()->Set_Select(m_pTarget);
		CSceneMgr::GetInstance()->Set_SelectedObject(m_pTarget);
	}
}

void CPickingMgr::LateUpdate_Picking(_float& dt)
{
	if (CInputMgr::GetInstance()->Mouse_Hold(DIM_LB)) return;

	if (m_PickTarget.empty()) m_pTarget = nullptr;

	auto it = min_element(m_PickTarget.begin(), m_PickTarget.end(),
		[](const RAYHIT& a, const RAYHIT& b) {
			return a.fDistance < b.fDistance;
		});

	if (it != m_PickTarget.end()) {
		float minDist = it->fDistance;
		m_pTarget = it->target;
	}

	m_PickTarget.clear();
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
	//NDC 좌표로 변환
	// 만약 x가 600이라면 (뷰포트 맨 왼쪽 - 마우스 포지션 / 뷰포트 넓이) 
	//이러면 0부터 1까지 사이에 어디에 위치한지가 나옴.
	//그때 2를 곱하면 0부터 2까지 사이에 어디에 위치한지가 나옴
	//이때 1을 빼면 -1~1사이에 위치한지가 나옴.
	ndc.x = ((float)m_CursorPos.x / (float)ViewPort.Width) * 2.0f - 1.0f;
	ndc.y = 1.0f - ((float)m_CursorPos.y / (float)ViewPort.Height) * 2.0f;
	ndc.z = 1.0f;

	// 투영 -> 뷰 스페이스
	D3DXMATRIX	matProj;
	matProj = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ProjMatrix();
	D3DXMatrixInverse(&matProj, 0, &matProj); //역행렬 돌려줌.
	D3DXVec3TransformCoord(&ndc, &ndc, &matProj); //그 역행렬을 기준으로 ndc를 계산함.
	//이러면 뷰 스페이스가 나오게됨 (즉 카메라 원점으로 돌린 그 공간)

	// 뷰 스페이스 -> 월드
	D3DXMATRIX	matView;
	matView = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ViewMatrix();
	D3DXMatrixInverse(&matView, 0, &matView);

	m_tRay.vRayPos = { 0.f, 0.f, 0.f }; //이건 카메라의 위치다. 왜냐면, 카메라는 원점으로 돌리고 z1을 바라보니까(뷰스페이스 상황)
	//클릭을 하게 되면 카메라에서 나오는거니까-> 카메라 위치에서 광선이 나오게 되는 것이 맞다.
	m_tRay.vRayDir = ndc - m_tRay.vRayPos; //그렇기에 지금 카메라에서 ndc로 향하는 방향을 구한다

	D3DXVec3TransformCoord(&m_tRay.vRayPos, &m_tRay.vRayPos, &matView); //레이포즈를 월드 위치 벡터로 변환
	D3DXVec3TransformNormal(&m_tRay.vRayDir, &m_tRay.vRayDir, &matView); //레이 디렉션을 월드 방향 벡터로 변환
	D3DXVec3Normalize(&m_tRay.vRayDir, &m_tRay.vRayDir);
}

void CPickingMgr::Free()
{
	Safe_Release(m_pDevice);
}
