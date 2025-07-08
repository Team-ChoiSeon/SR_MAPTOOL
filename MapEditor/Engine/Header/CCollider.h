#pragma once
#include "CComponent.h"
BEGIN(Engine)

class ENGINE_DLL CCollider :
    public CComponent
{
private:
    explicit CCollider();
     virtual~CCollider();
public:
    static CCollider* Create();
    HRESULT Ready_Component() override;
    void Update_Component(_float& dt) override;
    void LateUpdate_Component(_float& dt) override;
    
    void Render_Panel(ImVec2 size) override;
    void Render_Debug(LPDIRECT3DDEVICE9 pDevice);
    CComponent* Clone() const override;

 
    void Serialize(json& outJson) const override;
    void Deserialize(const json& inJson) override;
    string Get_ComponentName() const override; 
public:
    void On_CollisionEnter(CCollider* other);
    void On_CollisionStay(CCollider* other);
    void On_CollisionExit(CCollider* other);

public:
    AABB Get_AABBBox(){return m_tAABB; }
private:
    AABB m_tAABB = { {-1,-1,-1},{1,1,1} };
    AABB m_tAABBWorld;
    _vec3 offSetMin = {0,0,0};
    _vec3 offSetMax = { 0,0,0 };

    LPDIRECT3DVERTEXBUFFER9 m_pVB = nullptr;
    LPDIRECT3DINDEXBUFFER9  m_pIB = nullptr;

    //ColliderType m_eType = ColliderType::ACTIVE;
    //ColliderTag m_eTag = ColliderTag::NONE;

private:
    void Free() override;
};

END