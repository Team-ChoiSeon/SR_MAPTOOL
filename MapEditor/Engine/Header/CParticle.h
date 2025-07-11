#pragma once
#include "CComponent.h"
class CParticle :
    public CComponent
{
private:
    explicit CParticle();
    virtual ~CParticle();
public:
    static CParticle* Create();
public:
    HRESULT Ready_Component() override;
    void Update_Component(_float& dt) override;
    void LateUpdate_Component(_float& dt) override;
    void Render_Particle(LPDIRECT3DDEVICE9 pDevice);
    void Render_Panel(ImVec2 size) override;

    CComponent* Clone() const override;
    void Serialize(json& outJson) const override;
    void Deserialize(const json& inJson) override;
    string Get_ComponentName() const override;
private:
    string m_strEffectName;
    _bool m_bLoop;
    _float m_fDuration;
    _float m_fSpawnRate;
    _float m_fLifeTime;
    _vec3 m_vStartColor;//시작 색상
    _vec3 m_vEndColor;//종료 색상

    _vec3 m_vStartScale; //시작 크기
    _vec3 m_vEndScale; // 종료 크기


private:
    void Free() override;
};

