#pragma once
#include "CBase.h"
BEGIN(Engine)
class CGameObject;
class CTransform;
class CInputSystem;

class ENGINE_DLL CEditorSystem :
    public CBase
{
    DECLARE_SINGLETON(CEditorSystem)
public:
    explicit CEditorSystem();
    virtual ~CEditorSystem();
public:
    HRESULT Ready_EditorSystem();
    void Update_Editor(_float& dt);
    void LateUpdate_Editor(_float& dt);
    void Render_Gizmo();

public:
    void Set_Select(CGameObject* obj);
private:
    void Key_Check();

private:
    CGameObject* m_selected = nullptr;
    _matrix m_gizmoWorld;
    bool m_bEditing = false;
    LPDIRECT3DDEVICE9 m_pDevice;
    ImGuizmo::OPERATION gizmoOperation;
    D3DVIEWPORT9 vp;
    CTransform* m_pTransform;
    CInputSystem* m_pInputSystem = nullptr; // ก็
private:
    void Free();
};
END
