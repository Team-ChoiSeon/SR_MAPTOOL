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
	ScreenToClient(m_hwnd, &pt);  // ��ũ�� �� Ŭ���̾�Ʈ
	m_CursorPos = pt;

	if (GUISystem::GetInstance()->UsingUI()) return;

	if (CInputMgr::GetInstance()->Mouse_Down(DIM_RB))
		m_pTarget = nullptr;

	// ����� ���� ���̸� ��ŷ ����
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
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9)); //dx ����Ʈ ��������
	m_pDevice->GetViewport(&ViewPort);

	_vec3 ndc;
	//NDC ��ǥ�� ��ȯ
	// ���� x�� 600�̶�� (����Ʈ �� ���� - ���콺 ������ / ����Ʈ ����) 
	//�̷��� 0���� 1���� ���̿� ��� ��ġ������ ����.
	//�׶� 2�� ���ϸ� 0���� 2���� ���̿� ��� ��ġ������ ����
	//�̶� 1�� ���� -1~1���̿� ��ġ������ ����.
	ndc.x = ((float)m_CursorPos.x / (float)ViewPort.Width) * 2.0f - 1.0f;
	ndc.y = 1.0f - ((float)m_CursorPos.y / (float)ViewPort.Height) * 2.0f;
	ndc.z = 1.0f;

	// ���� -> �� �����̽�
	D3DXMATRIX	matProj;
	matProj = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ProjMatrix();
	D3DXMatrixInverse(&matProj, 0, &matProj); //����� ������.
	D3DXVec3TransformCoord(&ndc, &ndc, &matProj); //�� ������� �������� ndc�� �����.
	//�̷��� �� �����̽��� �����Ե� (�� ī�޶� �������� ���� �� ����)

	// �� �����̽� -> ����
	D3DXMATRIX	matView;
	matView = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ViewMatrix();
	D3DXMatrixInverse(&matView, 0, &matView);

	m_tRay.vRayPos = { 0.f, 0.f, 0.f }; //�̰� ī�޶��� ��ġ��. �ֳĸ�, ī�޶�� �������� ������ z1�� �ٶ󺸴ϱ�(�佺���̽� ��Ȳ)
	//Ŭ���� �ϰ� �Ǹ� ī�޶󿡼� �����°Ŵϱ�-> ī�޶� ��ġ���� ������ ������ �Ǵ� ���� �´�.
	m_tRay.vRayDir = ndc - m_tRay.vRayPos; //�׷��⿡ ���� ī�޶󿡼� ndc�� ���ϴ� ������ ���Ѵ�

	D3DXVec3TransformCoord(&m_tRay.vRayPos, &m_tRay.vRayPos, &matView); //������� ���� ��ġ ���ͷ� ��ȯ
	D3DXVec3TransformNormal(&m_tRay.vRayDir, &m_tRay.vRayDir, &matView); //���� �𷺼��� ���� ���� ���ͷ� ��ȯ
	D3DXVec3Normalize(&m_tRay.vRayDir, &m_tRay.vRayDir);
}

void CPickingMgr::Free()
{
	Safe_Release(m_pDevice);
}
