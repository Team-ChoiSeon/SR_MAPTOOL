#pragma once
#include "CComponent.h"
BEGIN(Engine)

class ENGINE_DLL CInputSystem :
    public CComponent
{
public: 
    explicit CInputSystem();
    virtual ~CInputSystem();
public:
    static CInputSystem* Create();
    HRESULT Ready_Component() override;
    void Update_Component(_float& dt) override;
    void LateUpdate_Component(_float& dt) override;
    void Render_Panel(ImVec2 size) override;
    CComponent* Clone() const override;
    void Serialize(json& outJson) const override;
    void Deserialize(const json& inJson) override;
    string Get_ComponentName() const override;

    _bool KeyBoadr_Control() { return m_bMove; };
private:
    _bool m_bMove;
    _float m_fMoveSpeed;
    _float m_fRotateSpeed;

private:
    void Free() override;
};

END