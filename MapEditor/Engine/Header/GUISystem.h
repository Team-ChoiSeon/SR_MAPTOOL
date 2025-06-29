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
    void Render_GUI();

    bool Set_Input(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void RegisterPanel(const string& name, function<void()> callback);
    void RemovePanel(const string& name);
    
private:
    LPDIRECT3DDEVICE9 m_pDevice;
    unordered_map<string, PANEL> m_PanelContainer;
private:
    void Free();

};

END