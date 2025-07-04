#include "Engine_Define.h"
#include "CScene.h"
#include "CLayer.h"
#include "CGameObject.h"
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
	ImGui::Begin("Scene Panel", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Scene: %s", m_Name.c_str());
	ImGui::Separator();

	for (const auto& pair : m_mapLayer)
	{
		string id = pair.first;
		CLayer* pLayer = pair.second;
		
		if (!pLayer || id.empty())
			continue;

		if (ImGui::TreeNode(id.c_str()))
		{
			pLayer->Render_Panel(); // 오브젝트 리스트 패널 출력
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
		return nullptr;
	else
		return (iter->second);
}

void CScene::Serialize(json& jScene) const
{
}

void CScene::Deserialize(const json& jScene)
{
}


void CScene::Free()
{
}
