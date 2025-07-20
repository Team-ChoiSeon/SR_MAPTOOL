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
#include "CPickingMgr.h"
#include "CPrefabMgr.h"
#include "CEditorSystem.h"
#include "CShaderMgr.h"
#include "CLightMgr.h"
#include "CCollisionMgr.h"
#include "CPhysicsMgr.h"

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

	m_pDeviceClass->AddRef();
	m_pGraphicDev = m_pDeviceClass->Get_GraphicDev();
	m_pGraphicDev->AddRef();

	if (FAILED(CResourceMgr::GetInstance()->Ready_Resource()))
		return E_FAIL;
	if (FAILED(CShaderMgr::GetInstance()->Ready_Shader(m_pGraphicDev)))
		return E_FAIL;
	if (FAILED(CPrefabMgr::GetInstance()->Ready_Prefabs()))
		return E_FAIL;
	if (FAILED(CInputMgr::GetInstance()->Ready_InputDev(g_HInst, g_hWnd)))
		return E_FAIL;
	if (FAILED(CRenderMgr::GetInstance()->Ready_RenderMgr(m_pGraphicDev)))
		return E_FAIL;
	if (FAILED(GUISystem::GetInstance()->Ready_GUI(g_hWnd)))
		return E_FAIL;	
	if (FAILED(CSceneMgr::GetInstance()->Ready_SceneMgr()))
		return E_FAIL;	
	if (FAILED(CCameraMgr::GetInstance()->Ready_CameraMgr()))
		return E_FAIL;		
	if (FAILED(CLightMgr::GetInstance()->Ready_Light(m_pGraphicDev)))
		return E_FAIL;
	if (FAILED(CPickingMgr::GetInstance()->Ready_Picking(g_hWnd)))
		return E_FAIL;	
	if (FAILED(CEditorSystem::GetInstance()->Ready_EditorSystem()))
		return E_FAIL;
	if(FAILED(CCollisionMgr::GetInstance()->Ready_Collision()))
		return E_FAIL;
	if (FAILED(CPhysicsMgr::GetInstance()->Ready_Physics()))
		return E_FAIL;

	return S_OK;
}

int CMainApp::Update_MainApp(_float& fTimeDelta)
{
	CInputMgr::GetInstance()->Update_InputDev();
	CPickingMgr::GetInstance()->Update_Picking(fTimeDelta);
	CCameraMgr::GetInstance()->Update_Camera(fTimeDelta);
	CSceneMgr::GetInstance()->Update_Scene(fTimeDelta);
	GUISystem::GetInstance()->Update_GUI(fTimeDelta);
	CEditorSystem::GetInstance()->Update_Editor(fTimeDelta);
	CLightMgr::GetInstance()->Update_Light(fTimeDelta);
	CCollisionMgr::GetInstance()->Update_Collision(fTimeDelta);
	CPhysicsMgr::GetInstance()->Update_Physics(fTimeDelta);
	return 0;
}

void CMainApp::LateUpdate_MainApp(_float& fTimeDelta)
{
	CInputMgr::GetInstance()->LateUpdate_InputDev();
	CPickingMgr::GetInstance()->LateUpdate_Picking(fTimeDelta);
	CCameraMgr::GetInstance()->LateUpdate_Camera(fTimeDelta);
	CSceneMgr::GetInstance()->LateUpdate_Scene(fTimeDelta);
	GUISystem::GetInstance()->LateUpdate_GUI(fTimeDelta);
	CEditorSystem::GetInstance()->LateUpdate_Editor(fTimeDelta);
	CLightMgr::GetInstance()->LateUpdate_Light(fTimeDelta);
	CCollisionMgr::GetInstance()->LateUpdate_Collision(fTimeDelta);
	CPhysicsMgr::GetInstance()->LateUpdate_Physics(fTimeDelta);
}

void CMainApp::Render_MainApp()
{
	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));

	CRenderMgr::GetInstance()->Render(m_pGraphicDev);
	GUISystem::GetInstance()->Render_Begin();
	CSceneMgr::GetInstance()->Render_Scene();
	CCollisionMgr::GetInstance()->Render_Collider(m_pGraphicDev);
	GUISystem::GetInstance()->Render_GUI();
	CRenderMgr::GetInstance()->Render_GUI();
	CEditorSystem::GetInstance()->Render_Gizmo();
	//CPhysicsMgr::GetInstance()->Render_Physics();
	CLightMgr::GetInstance()->Render_Light();
	GUISystem::GetInstance()->Render_End();

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
	CPickingMgr::GetInstance()->DestroyInstance();
	CPrefabMgr::GetInstance()->DestroyInstance();
	CEditorSystem::GetInstance()->DestroyInstance();
	CShaderMgr::GetInstance()->DestroyInstance();
	CLightMgr::GetInstance()->DestroyInstance();
	CCollisionMgr::GetInstance()->DestroyInstance();
	CPhysicsMgr::GetInstance()->DestroyInstance();
	CGraphicDev::GetInstance()->DestroyInstance();
}
