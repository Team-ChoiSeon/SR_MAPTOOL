#include "Engine_Define.h"
#include "GUISystem.h"
#include "CGraphicDev.h"
#include "CSceneMgr.h"
#include "CScene.h"
#include "Engine_GUI.h"

IMPLEMENT_SINGLETON(GUISystem)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

GUISystem::GUISystem()
	:m_pDevice(nullptr)
{
}

GUISystem::~GUISystem()
{
    Free();
}

HRESULT GUISystem::Ready_GUI(HWND hwnd)
{
	m_pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();  // 또는 Light, Classic
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    if (!ImGui_ImplWin32_Init(hwnd))
        return E_FAIL;
    if (!ImGui_ImplDX9_Init(m_pDevice))
        return E_FAIL;

    return S_OK;
}

void GUISystem::Render_GUI()
{
    
    ImGui_ImplDX9_NewFrame();     
    ImGui_ImplWin32_NewFrame();   
    ImGui::NewFrame();          

    for (auto& pair : m_PanelContainer)
    {
        const string& name = pair.first;
        PANEL& panel = pair.second;

        if (!panel.bOpen)
            continue;

        //ImGui::Begin(name.c_str(), &panel.bOpen);
        if (panel.callback)
            panel.callback();
        //ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    m_PanelContainer.clear();
}

bool GUISystem::Set_Input(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
}

void GUISystem::RegisterPanel(const string& name, function<void()> callback)
{
    m_PanelContainer.emplace(name, PANEL(true,callback));
}

void GUISystem::RemovePanel(const string& name)
{
    auto iter = m_PanelContainer.find(name);
    if (iter != m_PanelContainer.end()) {
        m_PanelContainer.erase(iter);
    }
}

// 종료: 백엔드 및 컨텍스트 해제
void GUISystem::Free()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
