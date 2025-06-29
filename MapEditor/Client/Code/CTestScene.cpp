#include "pch.h"
#include "Engine_Define.h"
#include "CTestScene.h"
#include "CScenePanel.h"
#include "CTestCube.h"

#include "CCamera.h"
#include "CCameraActor.h"
#include "CCameraMgr.h"
#include "CLayer.h"

CTestScene::CTestScene()
{
}

CTestScene::~CTestScene()
{
}

CTestScene* CTestScene::Create()
{
	CTestScene* instance = new CTestScene;

	if (FAILED(instance->Ready_Scene())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CTestScene::Ready_Scene()
{
	Add_Panel<CScenePanel>();

	Create_Layer("Camera");
	Create_Layer("Object");

	m_mapLayer["Camera"]->Add_Object("Camera01", CCameraActor::Create());
	m_mapLayer["Object"]->Add_Object("Object01", CTestCube::Create());

	CCamera* cam = (m_mapLayer["Camera"]->Find_Object("Camera01"))->Get_Component<CCamera>();
	CCameraMgr::GetInstance()->Set_MainCamera(cam);

	return S_OK;
}


void CTestScene::Update_Scene(_float& dt)
{
	Update_Panel(dt);
	m_mapLayer["Camera"]->Update_Layer(dt);
	m_mapLayer["Object"]->Update_Layer(dt);
}

void CTestScene::LateUpdate_Scene(_float& dt)
{
	LateUpdate_Panel(dt);
	m_mapLayer["Camera"]->LateUpdate_Layer(dt);
	m_mapLayer["Object"]->LateUpdate_Layer(dt);
}

void CTestScene::Free()
{
	FreeUI();
	Free_AllLayer();
}

HRESULT CTestScene::Enter_Scene()
{
	return S_OK;
}

HRESULT CTestScene::Exit_Scene()
{
	return S_OK;
}
