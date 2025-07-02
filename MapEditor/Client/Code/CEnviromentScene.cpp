#include"pch.h"
#include "Engine_Define.h"
#include "CEnviromentScene.h"

CEnviromentScene::CEnviromentScene()
{
}

CEnviromentScene::~CEnviromentScene()
{
}

CEnviromentScene* CEnviromentScene::Create()
{
	CEnviromentScene* instance = new CEnviromentScene;

	if (FAILED(instance->Ready_Scene())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CEnviromentScene::Ready_Scene()
{
	return S_OK;
}

void CEnviromentScene::Update_Scene(_float& dt)
{
}

void CEnviromentScene::LateUpdate_Scene(_float& dt)
{
}

void CEnviromentScene::Free()
{
}

HRESULT CEnviromentScene::Enter_Scene()
{
	return S_OK;
}

HRESULT CEnviromentScene::Exit_Scene()
{
	return S_OK;
}
