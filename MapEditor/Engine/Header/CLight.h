#pragma once
#include "CComponent.h"

BEGIN(Engine)
class ENGINE_DLL CLight :
    public CComponent
{
private:
    explicit CLight();
    virtual ~CLight();
public:
    static CLight* Create();
public:
    HRESULT Ready_Component() override;
    void Update_Component(_float& dt) override;
    void LateUpdate_Component(_float& dt) override;
    void Render_Panel(ImVec2 size) override;
public:
    CComponent* Clone() const override;
    string Get_ComponentName() const override { return "CLight"; };
    void Serialize(json& outJson) const override;
    void Deserialize(const json& inJson) override;
    _vec3 Get_Pos() { return m_tLight.Position; }
    D3DLIGHT9* Get_Light() { return &m_tLight; }
private:
    void Render_Directional();
    void Render_Point();
    void Render_Spot();
private:
    D3DLIGHT9 m_tLight;
    int m_iOption;
private:
    void Free() override;
};

END