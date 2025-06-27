#include "Engine_Define.h"
#include "CScenePanel.h"
#include "Engine_GUI.h"
#include "CSceneMgr.h"
#include "CScene.h"

int CScenePanel::m_nowInex = 0;

CScenePanel::CScenePanel()
: isOpen(false)
{
}

CScenePanel::~CScenePanel()
{
}

CScenePanel* CScenePanel::Create()
{
	CScenePanel* instance = new CScenePanel;

	if (FAILED(instance->Ready_Panel())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CScenePanel::Ready_Panel()
{
	Set_List();
	return S_OK;
}

void CScenePanel::Update_Panel(_float& dt)
{
}

void CScenePanel::LateUpdate_Panel(_float& dt)
{
}

void CScenePanel::Render_Panel()
{
	const auto& map = CSceneMgr::GetInstance()->Get_SceneArr();

	m_sceneList.clear();
	for (const auto& pair : map)
		m_sceneList.push_back(pair.first);

	if (m_sceneList.empty())
		return;

	// m_sceneList에서 직접 c_str() 꺼내기
	ImGui::Begin("Scene_Select", &isOpen, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::BeginCombo("##SceneCombo", m_sceneList[m_nowInex].c_str()))
	{
		for (int i = 0; i < m_sceneList.size(); ++i)
		{
			bool is_selected = (m_nowInex == i); //현재 인덱스 -> 

			if (ImGui::Selectable(m_sceneList[i].c_str(), is_selected)) {
				// 사용자가 이번 프레임에 이 항목을 선택했을 때만 실행됨
				if (m_nowInex != i) {
					m_nowInex = i;
					Request_SceneChange(m_sceneList[i]);  // 여기서 씬 변경 실행
				}
			}
		}
		ImGui::EndCombo();
	}

	ImGui::End();
}


void CScenePanel::Set_List()
{

}

void CScenePanel::Request_SceneChange(const string& name)
{
	const auto& map = CSceneMgr::GetInstance()->Get_SceneArr();

	auto iter = map.find(name);
	if (iter == map.end())
		return; // 존재하지 않는 씬 이름이면 무시

	if (m_pOwner != iter->second)
	{
		CSceneMgr::GetInstance()->Set_Scene(name);
	}
}


void CScenePanel::Free()
{
}