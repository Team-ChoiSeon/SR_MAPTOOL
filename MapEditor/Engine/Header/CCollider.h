#pragma once
#include "CComponent.h"
BEGIN(Engine)

class CRigidBody;
class CTransform;

enum class ColliderType { PASSIVE, ACTIVE, TRIGGER };
enum class ColliderTag { NONE, GROUND, WALL, PLATFORM };
enum class BoundingType { AABB, OBB };
enum class ColliderState { NONE, ENTER, STAY, EXIT };

struct BoundInfo
{
    BoundingType eType = BoundingType::AABB;

    // AABB용
    _vec3 vMin;
    _vec3 vMax;
    // OBB용
    _vec3 vCenter;
    _vec3 vAxisX, vAxisY, vAxisZ; // 단위 벡터
    _vec3 vHalf; // 반지름 길이

    // World 변환된 최종 코너들 (렌더/충돌용)
    vector<_vec3> vCorners;

    void Calc_Transform(const _matrix& matWorld)
    {
        vCenter = *(_vec3*)&matWorld._41;

        vAxisX = *(_vec3*)&matWorld._11;
        vAxisY = *(_vec3*)&matWorld._21;
        vAxisZ = *(_vec3*)&matWorld._31;

        D3DXVec3Normalize(&vAxisX, &vAxisX);
        D3DXVec3Normalize(&vAxisY, &vAxisY);
        D3DXVec3Normalize(&vAxisZ, &vAxisZ);

        vCorners.clear();
        vCorners.push_back(vCenter + (-vAxisX * vHalf.x + -vAxisY * vHalf.y + -vAxisZ * vHalf.z)); // 0
        vCorners.push_back(vCenter + (vAxisX * vHalf.x + -vAxisY * vHalf.y + -vAxisZ * vHalf.z)); // 1
        vCorners.push_back(vCenter + (vAxisX * vHalf.x + vAxisY * vHalf.y + -vAxisZ * vHalf.z)); // 2
        vCorners.push_back(vCenter + (-vAxisX * vHalf.x + vAxisY * vHalf.y + -vAxisZ * vHalf.z)); // 3
        vCorners.push_back(vCenter + (-vAxisX * vHalf.x + -vAxisY * vHalf.y + vAxisZ * vHalf.z)); // 4
        vCorners.push_back(vCenter + (vAxisX * vHalf.x + -vAxisY * vHalf.y + vAxisZ * vHalf.z)); // 5
        vCorners.push_back(vCenter + (vAxisX * vHalf.x + vAxisY * vHalf.y + vAxisZ * vHalf.z)); // 6
        vCorners.push_back(vCenter + (-vAxisX * vHalf.x + vAxisY * vHalf.y + vAxisZ * vHalf.z)); // 7
    }
};

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

    void On_Collision_Enter(CCollider* pOther);
    void On_Collision_Stay(CCollider* pOther);
    void On_Collision_Exit(CCollider* pOther);

    bool Calc_Push_AABB(const AABB& a, const AABB& b, _vec3& push);
    bool Calc_Push_OBB(const BoundInfo& a, const BoundInfo& b, _vec3& push);
    void Handle_Ground(CCollider* pOther, const _vec3& push);

    void Serialize(json& outJson) const override;
    void Deserialize(const json& inJson) override;
    string Get_ComponentName() const override; 
public:
    void Set_ColType(ColliderType eType) { m_eType = eType; }
    void Set_ColTag(ColliderTag eTag) { m_eTag = eTag; }
    void Set_ColState(ColliderState eState) { m_eState = eState; }
    void Set_BoundType(BoundingType eType) { m_tBound.eType = eType; }
    // 각 축 방향으로의 반지름 길이
    void Set_Offset(const _vec3& vOffset)
    {
        m_tAABBOff.vMin = -vOffset;
        m_tAABBOff.vMax = vOffset;

        m_tBound.vHalf = vOffset;
    }

    ColliderType Get_ColType()			const { return m_eType; }
    ColliderTag  Get_ColTag()			const { return m_eTag; }
    ColliderState Get_ColState()		const { return m_eState; }
    const AABB& Get_AABBW() { return m_tAABBWorld; }
    const AABB& Get_Offset()			const { return m_tAABBOff; }
    const BoundInfo& Get_Bound()		const { return m_tBound; }
    const BoundingType& Get_BoundType() const { return m_tBound.eType; }


public:
    AABB Get_AABBBox(){return m_tAABB; }
private:
    AABB m_tAABB = { {-1,-1,-1},{1,1,1} };
    AABB m_tAABBOff = { {0, 0, 0}, {0, 0, 0} };
    AABB m_tAABBWorld;

    BoundInfo m_tBound;

    LPDIRECT3DVERTEXBUFFER9 m_pVB = nullptr;
    LPDIRECT3DINDEXBUFFER9  m_pIB = nullptr;

    ColliderType m_eType = ColliderType::ACTIVE;
    ColliderTag m_eTag = ColliderTag::NONE;
    ColliderState m_eState = ColliderState::NONE;

    CRigidBody* m_pRigid = nullptr;
    CTransform* m_pTransform = nullptr;

private:
    void Free() override;
};

END