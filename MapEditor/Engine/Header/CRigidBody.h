#pragma once
#include "CComponent.h"
BEGIN(Engine)

class CCollider;

class ENGINE_DLL CRigidBody :
    public CComponent
{
public:
    enum RigidbodyType
    {
        Static = 1,
        Kinematic,
        Dynamic=3
    };


private:
    explicit CRigidBody();
    virtual ~CRigidBody();
public:
    static CRigidBody* Create();
public:
    HRESULT Ready_Component() override;
    void Update_Component(_float& dt) override;
    void LateUpdate_Component(_float& dt) override;
    void Render_Panel(ImVec2 size) override;
    CComponent* Clone() const override;
    void Resolve_PhysicsCollision(CCollider* self, CCollider* other);
    void Apply_Gravity(_float& dt);
    void Intergrate(_float& dt);

public:
    void Serialize(json& outJson) const override;
    void Deserialize(const json& inJson) override;
    string Get_ComponentName() const override;
public:
    RigidbodyType Get_Type() { return m_eType; }
    _float Get_Mass() { return m_fMass; }
    _float Get_Friction() { return m_fFric; }
    _float Get_Bounce() { return m_fBnc; }

    _vec3 Get_Velocity() { return m_vVel; }
    _vec3 Get_Accel() { return m_vAcc; }
    _vec3 Get_ExternalForcel() { return m_vEforce; }

    _bool Use_Gravity() { return m_bUseGravity; }

public:
    void Set_Velocity(_vec3 vel) { m_vVel = vel; }
    void Set_ExternalForce(_vec3 eforce) { m_vEforce = eforce; }
private:
    _bool m_bUseGravity = true;

    _float      m_fMass = 1.f;                          // 질량
    _float      m_fFric = 0.f;                     // 마찰 계수
    _float      m_fBnc = 0.0f;                    // 탄성 계수

    _vec3       m_vVel = _vec3(0.f, 0.f, 0.f);     // 속도
    _vec3       m_vAcc = _vec3(0.f, 0.f, 0.f);        // 가속도

    _vec3       m_vAVel = _vec3(0.f, 0.f, 0.f);    // 각속도
    _vec3       m_vAAcc = _vec3(0.f, 0.f, 0.f);    // 각가속도

   _vec3       m_vEforce = _vec3(0.f, 0.f, 0.f);       // 외부 힘

   //_vec3       m_vTorque = _vec3(0.f, 0.f, 0.f);       // 회전 힘
   //_vec3       m_fInertia = _vec3(1.f, 1.f, 1.f);      // 관성모먼트

    RigidbodyType m_eType = RigidbodyType::Static;

private:
    void Free() override;

};

END