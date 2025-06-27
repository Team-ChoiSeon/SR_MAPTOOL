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
    static CModel* Create();

public:
    HRESULT Ready_Component() override;
    virtual void Update_Component(float& dt);
    virtual void LateUpdate_Component(float& dt);
    CComponent* Clone() const override;

private:
    IMesh* m_pMesh;

private:
    void Free() override;
};

END