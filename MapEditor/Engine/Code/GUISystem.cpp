#include "Engine_Define.h"
#include "GUISystem.h"
#include "CGraphicDev.h"
#include "CSceneMgr.h"
#include "CScene.h"

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
    io.DisplaySize = ImVec2(static_cast<float>(WINCX), static_cast<float>(WINCY));
    /*HDC screen = GetDC(nullptr);
    float dpi = GetDeviceCaps(screen, LOGPIXELSX);
    ReleaseDC(nullptr, screen);
    */
    ImFont* bigFont = io.Fonts->AddFontFromFileTTF("../../font/NanumSquareNeo-cBd.ttf", 24.0f, nullptr, io.Fonts->GetGlyphRangesKorean()); // 큰 폰트
    ImFont* regular = io.Fonts->AddFontFromFileTTF("../../font/NanumSquareNeo-bRg.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesKorean()); //
    io.FontDefault = regular; 
    m_fontContainer.insert({ "Bold",bigFont });
    m_fontContainer.insert({ "Regular",regular });

    ImGui::StyleColorsDark();  // 또는 Light, Classic
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    if (!ImGui_ImplWin32_Init(hwnd))
        return E_FAIL;
    if (!ImGui_ImplDX9_Init(m_pDevice))
        return E_FAIL;

    return S_OK;
}

void GUISystem::Update_GUI(_float& dt)
{
    if (!ImGui::IsAnyItemActive() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
        m_bUsingUI = false;
    }
    else {
        m_bUsingUI = true;
    }
}

void GUISystem::LateUpdate_GUI(_float& dt)
{
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

ImFont* GUISystem::Get_Font(const string& tag)
{
    auto iter = m_fontContainer.find(tag);
    if (iter != m_fontContainer.end()) {
        return iter->second;
    }
    return nullptr;
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

void GUISystem::Open_FileDialogue()
{
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog("", nullptr, &outPath);

    if (result == NFD_OKAY)
    {
        // 사용
        free(outPath);
    }
    else
    {
        // NFD_CANCEL, NFD_ERROR일 경우에도 outPath가 nullptr이 아닐 수 있다.
        if (outPath)
            free(outPath);

        if (result == NFD_ERROR)
        {
            const char* err = NFD_GetError();
            if (err) printf("NFD Error: %s\n", err);
        }
    }
}

const std::wstring& GUISystem::Open_FolderDialogue()
{
    static std::wstring folderPath = L""; // static 유지

    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_PickFolder(nullptr, &outPath);

    if (result == NFD_OKAY)
    {
        // UTF-8 → UTF-16 변환
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        folderPath = converter.from_bytes(outPath);

        printf("Selected folder: %s\n", outPath); // 디버깅용
        free(outPath);
    }
    else
    {
        folderPath = L"";
        if (result == NFD_CANCEL)
            printf("User cancelled.\n");
        else
            printf("Error: %s\n", NFD_GetError());
    }

    return folderPath;
}


// 종료: 백엔드 및 컨텍스트 해제
void GUISystem::Free()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
