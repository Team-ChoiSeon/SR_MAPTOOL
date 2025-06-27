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
    
private:
    LPDIRECT3DDEVICE9 m_pDevice;
    
private:
    void Free();

};

END