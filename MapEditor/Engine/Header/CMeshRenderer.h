#pragma once
#include "CRenderer.h"

BEGIN(Engine)

class ENGINE_DLL CMeshRenderer :
    public CRenderer
{
private:
    explicit CMeshRenderer();
    virtual ~CMeshRenderer() override;

public:
    static CMeshRenderer* Create();

public:
    HRESULT Ready_Component() override;
    void Update_Component(float& dt) override;
    void LateUpdate_Component(float& dt) override;
    void Render(LPDIRECT3DDEVICE9 pDevice) override;
    CComponent* Clone() const override;
    void Set_Mesh(const wstring& key);
    RENDER_PASS Get_RenderPass() override { return RENDER_PASS::RP_OPAQUE; };

private:
    void Free() override;
};

END