#include "Engine_Define.h"
#include "CScene.h"
#include "IPanel.h"

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

