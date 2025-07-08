#pragma once
#include "CBase.h"
BEGIN(Engine)
class CRigidBody;

class ENGINE_DLL CPhysicsMgr :
    public CBase
{
private:
    explicit CPhysicsMgr();
    virtual ~CPhysicsMgr();

public:
    DECLARE_SINGLETON(CPhysicsMgr)
public:
    HRESULT Ready_Physics();
    void Update_Physics(_float& dt);
    void LateUpdate_Physics(_float& dt);
    void Render_Physics();
public:
    void Register_Physics(CRigidBody* rigid);
private:
    vector<CRigidBody*> m_RigidContainer;
    _bool m_bSimulate =false;
    _vec3 m_Gravity = _vec3(0.f, -9.8f, 0.f);
private:
    void Free() override;
};
END
