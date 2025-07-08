#pragma once
#include "CBase.h"
BEGIN(Engine)
class CCollider;

using ColliderPair = pair<CCollider*, CCollider*>;

class ENGINE_DLL CCollisionMgr :
    public CBase
{
private:
    explicit CCollisionMgr();
   virtual ~CCollisionMgr();

public:
    DECLARE_SINGLETON(CCollisionMgr)
public:
    HRESULT Ready_Collision();
    void Update_Collision(_float& dt);
    void LateUpdate_Collision(_float& dt);
    void Render_Collider(LPDIRECT3DDEVICE9 pDevice);
public:
    void Register_Collider(CCollider* collider);
private:
    bool IsAABBOverlap(const AABB& a, const AABB& b);
    inline pair<CCollider*, CCollider*> MakeSortedPair(CCollider* a, CCollider* b)
    {
        return (a < b) ? make_pair(a, b) : make_pair(b, a);
    }

private:
    vector<CCollider*> m_ColliderContainer;
    set<ColliderPair> m_CurrCollisionPairs;
    set<ColliderPair> m_PrevCollisionPairs;    

private:
    void Free() override;
};

END