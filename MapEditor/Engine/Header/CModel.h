#pragma once
#include "CComponent.h"
BEGIN(Engine)

class IMesh;

class ENGINE_DLL CModel :
    public CComponent
{
private:
    explicit CModel();
    virtual ~CModel();
public:
    static CModel* Create(const string& meshType);

public:
    HRESULT Ready_Component() override;
    virtual void Update_Component(float& dt);
    virtual void LateUpdate_Component(float& dt);
    void Render();

    CComponent* Clone() const override;
private:
    HRESULT Set_Mesh(const string& meshType);

private:
    IMesh* m_pMesh;

private:
    void Free() override;
};

END