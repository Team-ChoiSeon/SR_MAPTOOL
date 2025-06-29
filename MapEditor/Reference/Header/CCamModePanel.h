#pragma once
#include "IPanel.h"

BEGIN(Engine)
class ENGINE_DLL CCamModePanel :
    public IPanel
{
private:
    explicit CCamModePanel();
    virtual ~CCamModePanel();
public:
    static CCamModePanel* Create();

public:
    HRESULT Ready_Panel() override;
    void Update_Panel(_float& dt) override;
    void LateUpdate_Panel(_float& dt) override;
    void Render_Panel() override;
private:
    _bool m_bOpen;
    //CGameObject* m_pTargetCam;

private:
    void Free() override;
};

END