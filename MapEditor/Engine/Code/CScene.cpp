#include "Engine_Define.h"
#include "CScene.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CSceneMgr.h"
#include "CFunction.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

CScene* CScene::Create()
{
	CScene* instance = new CScene;

	if (FAILED(instance->Ready_Scene())) {
		Safe_Release(instance);
		instance = nullptr;
	}
	instance->Init_Layer();
	return instance;
}

HRESULT CScene::Ready_Scene()
{
	return S_OK;
}

void CScene::Update_Scene(_float& dt)
{
	for (auto& pair : m_mapLayer)
	{
		if (pair.second)
			pair.second->Update_Layer(dt);
	}
}

void CScene::LateUpdate_Scene(_float& dt)
{
	for (auto& pair : m_mapLayer)
	{
		if (pair.second)
			pair.second->LateUpdate_Layer(dt);
	}
}

void CScene::Render_Panel()
{
	ImVec2 panelPos = ImVec2(WINCX - 300, 0);                 // 오른쪽 상단
	ImVec2 panelSize = ImVec2(300, static_cast<float>(WINCY)); // 세로 전체

	ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(panelSize, ImGuiCond_Always);

	ImGui::Begin("Scene Panel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	ImGui::Text("Scene: %s", m_Name.c_str());

	ImGui::Separator();

	// === 레이어 목록 출력
	for (const auto& pair : m_mapLayer)
	{
		const string& id = pair.first;
		CLayer* pLayer = pair.second;

		if (!pLayer || id.empty())
			continue;

		if (ImGui::TreeNode(id.c_str()))
		{
			if (!pLayer->Get_Object().empty())
				pLayer->Render_Panel();

			ImGui::TreePop();
		}
	}

	ImGui::End();
}


void CScene::Init_Layer()
{
	Add_Layer("Default");
}

void CScene::Add_Layer(const string& layerName)
{
	if (m_mapLayer.find(layerName) == m_mapLayer.end())
	{
		CLayer* layer = CLayer::Create();
		m_mapLayer[layerName] = layer;
		m_LayerNames.push_back(layerName);
	}
}

void CScene::Free_Layer(string layer)
{
	auto iter = m_mapLayer.find(layer);

	if (iter == m_mapLayer.end()) return;

	Safe_Release(iter->second);
	m_mapLayer.erase(iter);
}

void CScene::Free_AllLayer()
{
	for (auto& pair : m_mapLayer) {
		Safe_Release(pair.second);
		pair.second = nullptr;
	}

	m_mapLayer.clear();
}

HRESULT CScene::Add_Object(string layer, CGameObject* object)
{
	if (FAILED(m_mapLayer[layer]->Add_Object(object))) {
		return E_FAIL;
	};
	return S_OK;
}

CLayer* CScene::Get_Layer(string id)
{
	auto iter = m_mapLayer.find(id);

	if (iter == m_mapLayer.end())
		return m_mapLayer["Default"];
	else
		return (iter->second);
}


void CScene::Serialize(json& jScene) const
{
	jScene["scene_name"] = m_Name;

	json jLayers;
	for (auto& [layerName, pLayer] : m_mapLayer)
	{
		json jLayer;
		pLayer->Serialize(jLayer);
		jLayers[layerName] = jLayer;
	}
	jScene["layers"] = jLayers;
}

void CScene::Deserialize(const json& jScene)
{
	// 씬 이름 불러오기
	if (jScene.contains("scene_name"))
		m_Name = jScene["scene_name"].get<string>();

	// 기존 레이어 초기화
	Free_AllLayer();
	m_mapLayer.clear();

	// layers 순회
	if (jScene.contains("layers"))
	{
		for (auto& [layerName, jLayer] : jScene["layers"].items())
		{
			CLayer* pLayer = CLayer::Create();
			if (!pLayer) continue;

			pLayer->Set_Name(layerName);
			pLayer->Deserialize(jLayer); // 레이어 역직렬화
			m_mapLayer[layerName] = pLayer;
			
			auto iter = find_if(m_LayerNames.begin(), m_LayerNames.end(), [&layerName](auto names)->bool {return names == layerName;});
			if(iter == m_LayerNames.end())
				m_LayerNames.push_back(layerName);
		}
	}
}


void CScene::Free()
{
	Free_AllLayer();
}
