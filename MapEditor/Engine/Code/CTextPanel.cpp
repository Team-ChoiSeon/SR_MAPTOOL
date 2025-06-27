#include "Engine_Define.h"
#include "CTextPanel.h"
#include "Engine_GUI.h"

CTextPanel::CTextPanel()
{
}

CTextPanel::~CTextPanel()
{
}

CTextPanel* CTextPanel::Create()
{
	CTextPanel* instance = new CTextPanel;

	if (FAILED(instance->Ready_Panel())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CTextPanel::Ready_Panel()
{
	return S_OK;
}

void CTextPanel::Update_Panel(_float& dt)
{
}

void CTextPanel::LateUpdate_Panel(_float& dt)
{
}

void CTextPanel::Render_Panel() {

	ImGui::Begin("TextPanel",nullptr,ImGuiChildFlags_AutoResizeX);
	ImGui::TextWrapped("%s", m_szText.c_str());
	
	const char* items[] = { 
		"Item 1",
		"Item 2", 
		"Item 3", "Item 4", "Item 5", "Item 6", "Item 7", "Item 8", "Item 9", "Item 10" };

	static int item_current_idx = 0;
	ImGui::ListBox("Listbox", &item_current_idx, items, IM_ARRAYSIZE(items), 4); // 4개의 항목이 표시됨
	ImGui::End();
}

void CTextPanel::Free()
{
}
