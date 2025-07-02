#pragma once
#include "CBase.h"
BEGIN(Engine)

class ENGINE_DLL GUISystem :
    public CBase
{
    DECLARE_SINGLETON(GUISystem)

private:
    explicit GUISystem();
    virtual ~GUISystem();

public:
    HRESULT Ready_GUI(HWND hwnd);
    void Update_GUI(_float& dt);
    void LateUpdate_GUI(_float& dt);
    void Render_GUI();
    ImFont* Get_Font(const string& tag);
    bool Set_Input(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void RegisterPanel(const string& name, function<void()> callback);
    void RemovePanel(const string& name);
    _bool UsingUI() { return m_bUsingUI; }
    const wstring& Open_FileDialogue();
    const wstring& Open_FolderDialogue();
private:
    LPDIRECT3DDEVICE9 m_pDevice;
    unordered_map<string, PANEL> m_PanelContainer;
    unordered_map<string, ImFont*> m_fontContainer;
    bool m_bUsingUI;

private:
    void Free();

};

END