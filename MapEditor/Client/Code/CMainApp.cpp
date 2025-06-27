#include "pch.h"
#include "Engine_Define.h"
#include "CMainApp.h"

#include "CGraphicDev.h"

#include "CTimeMgr.h"
#include "CFrameMgr.h"
#include "CInputMgr.h"
#include "CRenderMgr.h"
#include "GUISystem.h"
#include "CSceneMgr.h"
#include "CTestScene.h"
#include "CEnviromentScene.h"

CMainApp::CMainApp()
	:m_pDeviceClass(nullptr)
	, m_pGraphicDev(nullptr)
{
}

CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Ready_MainApp()
{
	if (FAILED(CGraphicDev::GetInstance()->Ready_GraphicDev(g_hWnd, MODE_WIN, WINCX, WINCY, &m_pDeviceClass)))
		return E_FAIL;

	if (FAILED(CInputMgr::GetInstance()->Ready_InputDev(g_HInst, g_hWnd)))
		return E_FAIL;

	if (FAILED(CRenderMgr::GetInstance()->Ready_RenderMgr(m_pGraphicDev)))
		return E_FAIL;
	if (FAILED(GUISystem::GetInstance()->Ready_GUI(g_hWnd)))
		return E_FAIL;	
	if (FAILED(CSceneMgr::GetInstance()->Ready_SceneMgr()))
		return E_FAIL;

	m_pDeviceClass->AddRef();

	m_pGraphicDev = m_pDeviceClass->Get_GraphicDev();
	m_pGraphicDev->AddRef();

	CSceneMgr::GetInstance()->Add_Scene("Test", CTestScene::Create());
	CSceneMgr::GetInstance()->Add_Scene("Enviroment", CTestScene::Create());
	CSceneMgr::GetInstance()->Set_Scene("Test");
	return S_OK;
}

int CMainApp::Update_MainApp(_float& fTimeDelta)
{
	CInputMgr::GetInstance()->Update_InputDev();
	CSceneMgr::GetInstance()->Update_Scene(fTimeDelta);

	return 0;
}

void CMainApp::LateUpdate_MainApp(_float& fTimeDelta)
{
	CInputMgr::GetInstance()->LateUpdate_InputDev();
	CSceneMgr::GetInstance()->LateUpdate_Scene(fTimeDelta);
}

void CMainApp::Render_MainApp()
{
	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));

	// 2. 렌더 상태 기본 세팅
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE); // 조명 없으면 어두워져서 끔

	// 3. 카메라 설정
	D3DXMATRIX matWorld, matView, matProj;
	D3DXMatrixIdentity(&matWorld);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXVECTOR3 vEye(0.f, 0.f, -5.f);
	D3DXVECTOR3 vAt(0.f, 0.f, 0.f);
	D3DXVECTOR3 vUp(0.f, 1.f, 0.f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.f), 800.f / 600.f, 0.1f, 100.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	

	// 5. 나머지 렌더 (GUI 등)
	GUISystem::GetInstance()->Render_GUI();
	// 6. 렌더 종료
	m_pDeviceClass->Render_End();
}


CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();
	if (FAILED(pInstance->Ready_MainApp())) {
		Engine::Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pDeviceClass);

	CTimeMgr::GetInstance()->DestroyInstance();
	CFrameMgr::GetInstance()->DestroyInstance();
	CInputMgr::GetInstance()->DestroyInstance();
	CRenderMgr::GetInstance()->DestroyInstance();
	GUISystem::GetInstance()->DestroyInstance();
	CSceneMgr::GetInstance()->DestroyInstance();

	CGraphicDev::GetInstance()->DestroyInstance();
}
