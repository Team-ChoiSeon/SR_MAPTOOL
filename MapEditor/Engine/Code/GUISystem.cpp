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

    // 3. 렌더 + 제출
    CScene* nowScene = CSceneMgr::GetInstance()->Get_NowScene();

   if(nowScene)
       nowScene->Render_Panel();

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

bool GUISystem::Set_Input(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
}

// 종료: 백엔드 및 컨텍스트 해제
void GUISystem::Free()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
