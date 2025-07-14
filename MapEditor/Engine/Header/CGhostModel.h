#pragma once
#include "CComponent.h"

BEGIN(Engine)
class ENGINE_DLL CGhostModel :
    public CComponent
{
private:
    explicit CGhostModel();
    virtual ~CGhostModel();
public:
    static CGhostModel* Create();

    // CComponent을(를) 통해 상속됨
    HRESULT Ready_Component() override;
    void Update_Component(_float& dt) override;
    void LateUpdate_Component(_float& dt) override;
    void Render(LPDIRECT3DDEVICE9 pDevice);

    void Render_Panel(ImVec2 size) override;
    CComponent* Clone() const override;
    void Serialize(json& outJson) const override;
    void Deserialize(const json& inJson) override;
    string Get_ComponentName() const override;
private:
    LPDIRECT3DVERTEXBUFFER9 m_pVB;
    LPDIRECT3DINDEXBUFFER9 m_pIB;
    LPDIRECT3DDEVICE9 m_pDevice;
private:
    void Free();
};
END
