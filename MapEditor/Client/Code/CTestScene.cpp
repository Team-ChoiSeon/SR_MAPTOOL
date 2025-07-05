#include "pch.h"
#include "Engine_Define.h"
#include "CTestScene.h"
#include "CLayer.h"

#include "CGameObject.h"
#include "CCamera.h"
#include "CTransform.h"
#include "CCameraMgr.h"

#include "CPickingMgr.h"
#include "GUISystem.h"
#include "CFunction.h"
#include "CModel.h"

CTestScene::CTestScene()
	:pTarget(nullptr)
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
	Init_Layer();
	return S_OK;
}


void CTestScene::Update_Scene(_float& dt)
{

	for (auto& pair : m_mapLayer) {
		pair.second->Update_Layer(dt);
	}

	GUISystem::GetInstance()->RegisterPanel("Object_Create", [this]() {Create_Object();});

	pTarget = CPickingMgr::GetInstance()->Get_PickedObj();
	GUISystem::GetInstance()->RegisterPanel("ObjectList", [this]() {Show_ObjectList();});

	if (pTarget)
		GUISystem::GetInstance()->RegisterPanel("ObjectEditor", [this]() {Edit_Object(pTarget);});
}

void CTestScene::LateUpdate_Scene(_float& dt)
{
	for (auto& pair : m_mapLayer) {
		pair.second->LateUpdate_Layer(dt);
	}
}

void CTestScene::Free()
{
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
