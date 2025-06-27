#include "pch.h"
#include "Engine_Define.h"
#include "CTestScene.h"
#include "CScenePanel.h"

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
	return S_OK;
}


void CTestScene::Update_Scene(_float& dt)
{
	Update_Panel(dt);
}

void CTestScene::LateUpdate_Scene(_float& dt)
{
	LateUpdate_Panel(dt);
}

void CTestScene::Free()
{
	FreeUI();
}