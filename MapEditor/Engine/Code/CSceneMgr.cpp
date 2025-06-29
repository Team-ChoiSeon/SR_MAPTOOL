#include "Engine_Define.h"
#include "CSceneMgr.h"
#include "CScene.h"
#include "GUISystem.h"

IMPLEMENT_SINGLETON(CSceneMgr)

CSceneMgr::CSceneMgr()
	:m_CurScene(nullptr),m_SceneIndex(0)
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
	if (m_CurScene) {
		GUISystem::GetInstance()->RegisterPanel("SceneTag", [this]() {
			Render_SceneSelector();
		});

		m_CurScene->Update_Scene(dt);
	}
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
	m_SceneList.push_back(sceneTag);
}

void CSceneMgr::Change_Scene(string sceneTag)
{
	auto iter = m_SceneContainer.find(sceneTag);

	if (iter == m_SceneContainer.end()) return;

	CScene* next = iter->second;

	if (m_CurScene) {
		HRESULT exit = m_CurScene->Exit_Scene();
		if (FAILED(exit)) {
			MessageBoxW(0, L"æ¿ ¿¸»Ø Ω«∆–, æ¿ ≈¿Â ø¿∑˘", L"error", MB_OK);
		}
	}

	HRESULT enter = next->Enter_Scene();

	if (FAILED(enter)) {
		MessageBoxW(0, L"æ¿ ¿¸»Ø Ω«∆–, æ¿ ¿‘¿Â ø¿∑˘", L"error", MB_OK);
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

void CSceneMgr::Render_SceneSelector()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	ImGui::Begin("Scene_Select", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	const std::string& currentSceneName = m_SceneList[m_SceneIndex];

	if (ImGui::BeginCombo("##SceneCombo", currentSceneName.c_str()))
	{
		for (int i = 0; i < m_SceneList.size(); ++i)
		{
			bool is_selected = (m_SceneIndex == i);
			if (ImGui::Selectable(m_SceneList[i].c_str(), is_selected)) {
				if (m_SceneIndex != i) {
					m_SceneIndex = i;
					Change_Scene(m_SceneList[i]);  // ø©±‚º≠ æ¿ ∫Ø∞Ê Ω««‡
				}
			}
		}
		ImGui::EndCombo();
	}
	ImGui::End();
}
