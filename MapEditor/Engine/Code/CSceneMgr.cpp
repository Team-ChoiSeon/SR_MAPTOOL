#include "Engine_Define.h"
#include "CSceneMgr.h"
#include "CScene.h"

IMPLEMENT_SINGLETON(CSceneMgr)

CSceneMgr::CSceneMgr()
	:m_CurScene(nullptr)
{
}

CSceneMgr::~CSceneMgr()
{
	Free();
}

HRESULT CSceneMgr::Ready_SceneMgr()
{
	return S_OK;
}

void CSceneMgr::Update_Scene(_float& dt)
{
}

void CSceneMgr::LateUpdate_Scene(_float& dt)
{
}

void CSceneMgr::Add_Scene(string sceneTag, CScene* scene)
{
	scene->Set_Name(sceneTag);
	m_SceneContainer.insert({ sceneTag,scene });
}

void CSceneMgr::Set_Scene(string sceneTag)
{
	if (m_CurScene == m_SceneContainer[sceneTag]) return;

	m_CurScene = m_SceneContainer[sceneTag];
}

void CSceneMgr::Free()
{
	for (auto& Pair : m_SceneContainer) {
		Safe_Release(Pair.second);
	}
	m_SceneContainer.clear();
}
