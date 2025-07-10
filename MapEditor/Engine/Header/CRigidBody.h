#pragma once
#include "CComponent.h"
BEGIN(Engine)

class CCollider;
class CTransform;

class ENGINE_DLL CRigidBody :
    public CComponent
{

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

public:
    void Serialize(json& outJson) const override;
    void Deserialize(const json& inJson) override;
    string Get_ComponentName() const override;
public:
    void Set_OnGround(bool bGround) { m_bGround = bGround; }
    void Set_UseGravity(bool bUse) { m_bGravity = bUse; }

    void Set_Mass(_float fMass) { m_fMass = fMass; }
    void Set_Friction(_float fFric) { m_fFric = fFric; }
    void Set_Bounce(_float fBnc) { m_fBnc = fBnc; }
    void Set_Gravity(_float fGrav) { m_fGravity = fGrav; }

    void Add_Force(const _vec3& vForce) { m_vEforce += vForce; }
    void Add_Torque(const _vec3& vTorque) { m_vTorque += vTorque; }
    void Add_Velocity(const _vec3& vVel) { m_vVel += vVel; }
    void Add_AVelocity(const _vec3& vAVel) { m_vAVel += vAVel; }

    void Set_Force(const _vec3& vForce) { m_vEforce = vForce; }
    void Set_AAccel(const _vec3& vAAcc) { m_vAAcc = vAAcc; }
    void Set_Torque(const _vec3& vTorque) { m_vTorque = vTorque; }
    void Set_Inertia(const _vec3& vInertia) { m_fInertia = vInertia; }
    void Set_Velocity(const _vec3& vVel) { m_vVel = vVel; }
    void Set_Accel(const _vec3& vAcc) { m_vAcc = vAcc; }
    void Set_AVelocity(const _vec3& vAVel) { m_vAVel = vAVel; }

    bool  Get_OnGround()   const { return m_bGround; }
    bool  Get_UseGravity() const { return m_bGravity; }

    _float Get_Mass()   const { return m_fMass; }
    _float Get_Friction() const { return m_fFric; }
    _float Get_Bounce() const { return m_fBnc; }
    _float Get_Gravity() const { return m_fGravity; }

    _vec3 Get_Velocity()   const { return m_vVel; }
    _vec3 Get_AVelocity()  const { return m_vAVel; }
    _vec3 Get_Accel()      const { return m_vAcc; }
    _vec3 Get_Gforce()     const { return m_vGforce; }
    _vec3 Get_Eforce()     const { return m_vEforce; }
    _vec3 Get_AAccel() const { return m_vAAcc; }
    _vec3 Get_Torque()     const { return m_vTorque; }
    _vec3 Get_Inertia() const { return m_fInertia; }
public:
    void Set_Velocity(_vec3 vel) { m_vVel = vel; }
    void Set_ExternalForce(_vec3 eforce) { m_vEforce = eforce; }
    void Set_Simulate(_bool simul) { m_bSimulate = simul; }
private:
    _bool m_bSimulate = false;
    _bool m_bUseGravity = true;

    _float      m_fMass = 1.f;                          // 질량
    _float      m_fFric = 0.f;                     // 마찰 계수
    _float      m_fBnc = 0.0f;                    // 탄성 계수
    _float      m_fGravity = 1.f;                   // 중력 계수

    _vec3       m_vVel = _vec3(0.f, 0.f, 0.f);     // 속도
    _vec3       m_vAcc = _vec3(0.f, 0.f, 0.f);        // 가속도
    _vec3       m_vAVel = _vec3(0.f, 0.f, 0.f);    // 각속도
    _vec3       m_vAAcc = _vec3(0.f, 0.f, 0.f);    // 각가속도

    _vec3       m_vGforce = _vec3(0.f, 0.f, 0.f);       // 중력 : m_vGforce = _vec3(0.f, -9.8f * m_fGravity, 0.f) * m_fMass;
    _vec3       m_vEforce = _vec3(0.f, 0.f, 0.f);       // 외부 힘
    _vec3       m_vTorque = _vec3(0.f, 0.f, 0.f);       // 회전 힘
    _vec3       m_fInertia = _vec3(1.f, 1.f, 1.f);      // 관성모먼트

    bool        m_bGravity = true;
    bool        m_bGround = false;


    CTransform* m_pTransform = nullptr;
private:
    void Free() override;

};

END