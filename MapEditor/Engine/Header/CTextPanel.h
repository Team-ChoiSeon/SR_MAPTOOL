#pragma once
#include "IPanel.h"
BEGIN(Engine)
class ENGINE_DLL CTextPanel :
    public IPanel
{
private:
    explicit CTextPanel();
    virtual ~CTextPanel();

public:
    static CTextPanel* Create();

public:
    HRESULT Ready_Panel() override;
    void Update_Panel(_float& dt) override;
    void LateUpdate_Panel(_float& dt) override;
    void Render_Panel() override;
    void Set_Text(const string& text) { m_szText = text; };
private:
    string m_szText;
private:
    void Free() override;
};

END