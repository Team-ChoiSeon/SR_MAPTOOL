#pragma once
#include "CBase.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CComponent :
    public CBase
{
protected:
    explicit CComponent() = default;
    virtual ~CComponent() =default;

public: 
    virtual HRESULT Ready_Component() PURE;
    virtual void Update_Component(_float& dt)PURE;
    virtual void LateUpdate_Component(_float& dt)PURE;
    virtual void Render_Panel(ImVec2 size)PURE;
    virtual CComponent* Clone() const PURE;
    virtual void Serialize(json& outJson) const PURE;
    virtual void Deserialize(const json& inJson) PURE;
public:
    bool Get_ComponentActive() { return m_bActive; }
    void Set_ComponentActive(bool active) { m_bActive = active; }
protected:
    bool m_bActive = true;
public:
    CGameObject* m_pOwner = nullptr;
};

END

