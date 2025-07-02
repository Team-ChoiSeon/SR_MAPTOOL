#include "Engine_Define.h"
#include "CScene.h"
#include "CLayer.h"
#include "CGameObject.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::Update_Scene(_float& dt)
{

}

void CScene::LateUpdate_Scene(_float& dt)
{

}

CLayer* CScene::Get_Layer(LAYER_ID id)
{
	auto iter = m_mapLayer.find(id);

	if (iter == m_mapLayer.end())
		return nullptr;
	else
		return (iter->second);
}

void CScene::Init_Layer()
{
	for (int i = 0; i < static_cast<int>(LAYER_ID::L_END); ++i) {
		LAYER_ID id = static_cast<LAYER_ID>(i);
		m_mapLayer[id] = CLayer::Create();
	}
}

void CScene::Swap_Layer(const string& from, const string& to)
{
	//미구현 사항
}


void CScene::Free_Layer(LAYER_ID layer)
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

HRESULT CScene::Add_Object(const string& tag, LAYER_ID layer, CGameObject* object)
{
	if (FAILED(m_mapLayer[layer]->Add_Object(object))) {
		return E_FAIL;
	};
	object->Set_Name(tag);
	object->Set_LayerID(layer);
	return S_OK;
}

const char* CScene::Layer_ToString(LAYER_ID id)
{
	switch (id)
	{
	case Engine::LAYER_ID::L_DEFAULT: return "DEFAULT";
	case Engine::LAYER_ID::L_CAMERA:  return "CAMERA";
	case Engine::LAYER_ID::L_OBJECT:  return "OBJECT";
	case Engine::LAYER_ID::L_PLAYER:  return "PLAYER";
	case Engine::LAYER_ID::L_TILE:    return "TILE";
	default:                          return "UnKnown";
	}
}


