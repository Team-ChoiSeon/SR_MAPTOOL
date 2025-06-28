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
	if (m_CurScene)
		m_CurScene->Update_Scene(dt);
}

void CSceneMgr::LateUpdate_Scene(_float& dt)
{
	if (m_CurScene)
		m_CurScene->LateUpdate_Scene(dt);
}

void CSceneMgr::Add_Scene(string sceneTag, CScene* scene)
{
	scene->Set_Name(sceneTag);
	m_SceneContainer.insert({ sceneTag,scene });
}

void CSceneMgr::Change_Scene(string sceneTag)
{
	auto iter = m_SceneContainer.find(sceneTag);

	if (iter == m_SceneContainer.end()) return;

	CScene* next = iter->second;

	if (m_CurScene) {
		HRESULT exit = m_CurScene->Exit_Scene();
		if (FAILED(exit)) {
			MessageBoxW(0, L"¾À ÀüÈ¯ ½ÇÆĞ, ¾À ÅğÀå ¿À·ù", L"error", MB_OK);
		}
	}

	HRESULT enter = next->Enter_Scene();

	if (FAILED(enter)) {
		MessageBoxW(0, L"¾À ÀüÈ¯ ½ÇÆĞ, ¾À ÀÔÀå ¿À·ù", L"error", MB_OK);
		return;
	}

	m_CurScene = next;
}

void CSceneMgr::Free()
{
	for (auto& Pair : m_SceneContainer) {
		Safe_Release(Pair.second);
	}
	m_SceneContainer.clear();
}
