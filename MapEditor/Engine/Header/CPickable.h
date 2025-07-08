#pragma once
#include "CComponent.h"
BEGIN(Engine)
class CTransform;
class CModel;

class ENGINE_DLL CPickable :
    public CComponent
{
private:
    explicit CPickable();
    virtual ~CPickable();

public:
    static CPickable* Create();
public:
    HRESULT Ready_Component() override;
    void Update_Component(_float& dt) override;
    void LateUpdate_Component(_float& dt) override;
    void Render_Panel(ImVec2 size) override;
    CComponent* Clone() const override;
    void Serialize(json& outJson) const override;
    void Deserialize(const json& inJson) override;
    string Get_ComponentName() const override;
private:
    RAY RayToLocal(RAY ray);
    _bool ComputeRay(RAY ray, float* distance);
    _bool ComputeRay_WorldSpace(RAY worldRay, CTransform* pTransform, float* distance);
private:
    CTransform* m_pTransform;
    CModel* m_pModel;
    RAYHIT myINFO;
    _bool m_bVaildInGame;
private:
    void Free() override;


};

END