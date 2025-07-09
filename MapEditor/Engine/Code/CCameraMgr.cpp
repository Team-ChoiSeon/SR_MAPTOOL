#include "Engine_Define.h"
#include "CCameraMgr.h"
#include "CTransform.h"
#include "CCamera.h"
#include "CInputSystem.h"
#include "CGraphicDev.h"
#include "CGameObject.h"
#include "CInputMgr.h"
#include "CSceneMgr.h"

IMPLEMENT_SINGLETON(CCameraMgr)

CCameraMgr::CCameraMgr()
	:m_pMainCam(nullptr)
{
}

CCameraMgr::~CCameraMgr()
{
	Free();
}

HRESULT CCameraMgr::Ready_CameraMgr()
{
	m_pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();
	m_pDefaultCam = CGameObject::CreateDeafult();
	m_pDefaultCam->Add_Component<CTransform>();
	m_pDefaultCam->Add_Component<CCamera>();

	CTransform* deftrans = m_pDefaultCam->Get_Component<CTransform>();
	CCamera* defCam = m_pDefaultCam->Get_Component<CCamera>();

	deftrans->Set_Pos({ 0.f, 3.f, -10.f });

	defCam->Set_View(
		{ 0.f, 3.f, -10.f },
		{ 0.f, 1.f, 0.f },
		{ 0.f, 0.f, 1.f }
	);
	defCam->Set_Proj(
		45.f, 0.1f, 1000.f
	);

	return S_OK;
}

void CCameraMgr::Update_Camera(_float& dt)
{
	_matrix viewMat;
	_matrix projMat;

	if (m_pMainCam) {
		viewMat = m_pMainCam->Get_ViewMatrix();
		projMat = m_pMainCam->Get_ProjMatrix();
		m_bDefault = false;
	}
	else {
		m_pDefaultCam->Update_GameObject(dt);
		CCamera* defCam = m_pDefaultCam->Get_Component<CCamera>();

		Move_DeafultCam(dt);

		viewMat = defCam->Get_ViewMatrix();
		 projMat = defCam->Get_ProjMatrix();
		 m_bDefault = true;
	}

	m_pDevice->SetTransform(D3DTS_VIEW, &viewMat);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &projMat);
}

void CCameraMgr::LateUpdate_Camera(_float& dt)
{
	m_pDefaultCam->LateUpdate_GameObject(dt);
}

void CCameraMgr::Set_MainCamera(CCamera* pCam)
{
	m_pMainCam = pCam;
}

CCamera* CCameraMgr::Get_MainCamera()
{
	if(m_pMainCam)
	 return m_pMainCam;

	else
		return m_pDefaultCam->Get_Component<CCamera>();
}

bool CCameraMgr::IsCameraInputActive() const
{
	return CInputMgr::GetInstance()->Key_Down(DIK_UP) ||
		CInputMgr::GetInstance()->Key_Down(DIK_DOWN) ||
		CInputMgr::GetInstance()->Key_Down(DIK_LEFT) ||
		CInputMgr::GetInstance()->Key_Down(DIK_RIGHT) ||
		CInputMgr::GetInstance()->Key_Down(DIK_LSHIFT) ||
		CInputMgr::GetInstance()->Key_Down(DIK_SPACE) ||
		CInputMgr::GetInstance()->Key_Down(DIK_LCONTROL);
}


void CCameraMgr::Move_DeafultCam(_float& dt)
{
	_vec3 dir = m_pDefaultCam->Get_Component<CCamera>()->Get_Dir();
	_vec3 up = m_pDefaultCam->Get_Component<CCamera>()->Get_Up();
	_vec3 right = m_pDefaultCam->Get_Component<CCamera>()->Get_Right();

	CTransform* transform = m_pDefaultCam->Get_Component<CTransform>();

	if (!CInputMgr::GetInstance()->Key_Down(DIK_LCONTROL)) {
		if (CInputMgr::GetInstance()->Key_Down(DIK_UP)) {
			transform->Add_Pos(dir * dt * 20);
		}
		if (CInputMgr::GetInstance()->Key_Down(DIK_DOWN)) {
			transform->Add_Pos(-dir * dt * 20);
		}
		if (CInputMgr::GetInstance()->Key_Down(DIK_RIGHT)) {
			_vec3 dir = { 1,0,0 };
			transform->Add_Pos(right * dt * 20);
		}
		if (CInputMgr::GetInstance()->Key_Down(DIK_LEFT)) {
			transform->Add_Pos(-right * dt * 20);
		}
		if (CInputMgr::GetInstance()->Key_Down(DIK_SPACE)) {
			transform->Add_Pos(up * dt * 20);
		}
		if (CInputMgr::GetInstance()->Key_Down(DIK_LSHIFT)) {
			_vec3 dir = { 0,-1,0 };
			transform->Add_Pos(-up * dt * 20);
		}
	}
	else {
		if (CInputMgr::GetInstance()->Key_Down(DIK_UP)) {
			transform->Add_Rotate(right *dt * 40);
		}
		if (CInputMgr::GetInstance()->Key_Down(DIK_DOWN)) {
			transform->Add_Rotate(-right * dt * 40);
		}
		if (CInputMgr::GetInstance()->Key_Down(DIK_RIGHT)) {
			transform->Add_Rotate(up * dt * 40);
		}
		if (CInputMgr::GetInstance()->Key_Down(DIK_LEFT)) {
			transform->Add_Rotate(-up * dt * 40);
		}
	}
}

void CCameraMgr::Free()
{
	Safe_Release(m_pDefaultCam);
}
