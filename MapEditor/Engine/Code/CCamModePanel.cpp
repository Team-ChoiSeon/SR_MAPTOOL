#include "Engine_Define.h"
#include "CCamModePanel.h"

void CCamModePanel::Free()
{
}

CCamModePanel::CCamModePanel()
{
}

CCamModePanel::~CCamModePanel()
{
}

CCamModePanel* CCamModePanel::Create()
{
	CCamModePanel* instance = new CCamModePanel;
	
	if (FAILED(instance->Ready_Panel())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CCamModePanel::Ready_Panel()
{
	return S_OK;
}

void CCamModePanel::Update_Panel(_float& dt)
{
}

void CCamModePanel::LateUpdate_Panel(_float& dt)
{
}

void CCamModePanel::Render_Panel()
{
	ImGui::SetNextWindowPos(ImVec2(10, 20), ImGuiCond_Always);
	ImGui::Begin("Camera Mode", &m_bOpen, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::End();
}
