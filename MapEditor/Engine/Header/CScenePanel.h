#pragma once
#include "IPanel.h"
BEGIN(Engine)
class ENGINE_DLL CScenePanel :
    public IPanel
{
private:
    explicit CScenePanel();
    virtual ~CScenePanel();
public:
    static CScenePanel* Create();
public:
    HRESULT Ready_Panel() override;
    void Update_Panel(_float& dt) override;
    void LateUpdate_Panel(_float& dt) override;
    void Render_Panel() override;

public:
    void Set_List();
    void Request_SceneChange(const string& name);

private:
    bool isOpen;
    static int m_nowInex;
    vector<string> m_sceneList;
private:
    void Free() override;
};

END