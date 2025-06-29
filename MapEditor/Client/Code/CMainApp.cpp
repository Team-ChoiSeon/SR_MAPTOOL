#include "pch.h"
#include "Engine_Define.h"
#include "CMainApp.h"

#include "CGraphicDev.h"

#include "CTimeMgr.h"
#include "CFrameMgr.h"
#include "CInputMgr.h"
#include "CRenderMgr.h"
#include "CCameraMgr.h"
#include "GUISystem.h"
#include "CSceneMgr.h"
#include "CResourceMgr.h"

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
	if (FAILED(CResourceMgr::GetInstance()->Ready_Resource()))
		return E_FAIL;	
	if (FAILED(CCameraMgr::GetInstance()->Ready_CameraMgr()))
		return E_FAIL;

	m_pDeviceClass->AddRef();

	m_pGraphicDev = m_pDeviceClass->Get_GraphicDev();
	m_pGraphicDev->AddRef();

	CSceneMgr::GetInstance()->Add_Scene("Test", CTestScene::Create());
	CSceneMgr::GetInstance()->Change_Scene("Test");

	CSceneMgr::GetInstance()->Add_Scene("Enviroment", CEnviromentScene::Create());
	return S_OK;
}

int CMainApp::Update_MainApp(_float& fTimeDelta)
{
	CInputMgr::GetInstance()->Update_InputDev();
	CCameraMgr::GetInstance()->Update_Camera(fTimeDelta);
	CSceneMgr::GetInstance()->Update_Scene(fTimeDelta);

	return 0;
}

void CMainApp::LateUpdate_MainApp(_float& fTimeDelta)
{
	CInputMgr::GetInstance()->LateUpdate_InputDev();
	CCameraMgr::GetInstance()->LateUpdate_Camera(fTimeDelta);
	CSceneMgr::GetInstance()->LateUpdate_Scene(fTimeDelta);
}

void CMainApp::Render_MainApp()
{
	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));
	CRenderMgr::GetInstance()->Render(m_pGraphicDev);
	GUISystem::GetInstance()->Render_GUI();
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
	CResourceMgr::GetInstance()->DestroyInstance();
	CCameraMgr::GetInstance()->DestroyInstance();
	CGraphicDev::GetInstance()->DestroyInstance();
}
