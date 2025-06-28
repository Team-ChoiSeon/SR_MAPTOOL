#include "Engine_Define.h"
#include "CScene.h"
#include "IPanel.h"
#include "CLayer.h"

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

void CScene::Render_Panel()
{
	for (auto& panel : m_UIContainer) {
		panel.second->Render_Panel();
	}
}

void CScene::Update_Panel(_float& dt)
{
	for (auto& panel : m_UIContainer) {
		panel.second->Update_Panel(dt);
	}
}

void CScene::LateUpdate_Panel(_float& dt)
{
	for (auto& panel : m_UIContainer) {
		panel.second->LateUpdate_Panel(dt);
	}
}

void CScene::FreeUI()
{
	for (auto& panel : m_UIContainer) {
		Safe_Release(panel.second);
	}
}

void CScene::Create_Layer(const string& key)
{
	if (m_mapLayer.count(key)) return;

	CLayer* tmp = CLayer::Create();
	m_mapLayer.insert({ key,tmp });

}

void CScene::Free_Layer(const string& key)
{
	auto iter = m_mapLayer.find(key);

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

