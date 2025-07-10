#include "Engine_Define.h"
#include "CCollisionMgr.h"
#include "CCollider.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}

CCollisionMgr::~CCollisionMgr()
{
    Free();
}

HRESULT CCollisionMgr::Ready_Collision()
{
	m_ColliderContainer.reserve(20);
	return S_OK;
}

void CCollisionMgr::Update_Collision(_float& dt)
{
    m_CurrCollisionPairs.clear();

    for (size_t i = 0; i < m_ColliderContainer.size(); ++i)
    {
        CCollider* a = m_ColliderContainer[i];
        if (!a || !a->Get_ComponentActive()) continue;

        const AABB& aBox = a->Get_AABBBox();

        for (size_t j = i + 1; j < m_ColliderContainer.size(); ++j)
        {
            CCollider* b = m_ColliderContainer[j];
            if (!b || !b->Get_ComponentActive()) continue;
            if (a->m_pOwner == b->m_pOwner) continue;

            const AABB& bBox = b->Get_AABBBox();

            if (IsAABBOverlap(aBox, bBox))
            {
                ColliderPair pair = MakeSortedPair(a, b);
                m_CurrCollisionPairs.insert(pair);

                if (m_PrevCollisionPairs.find(pair) != m_PrevCollisionPairs.end())
                {
                    // 유지 충돌
                    a->On_Collision_Stay(b);
                    b->On_Collision_Stay(a);
                }
                else
                {
                    // 새 충돌 시작
                    a->On_Collision_Enter(b);
                    b->On_Collision_Enter(a);
                }
            }
        }
    }

    // 이전 프레임에는 있었지만 이번엔 사라진 충돌
    for (const auto& pair : m_PrevCollisionPairs)
    {
        if (m_CurrCollisionPairs.find(pair) == m_CurrCollisionPairs.end())
        {
            pair.first->On_Collision_Exit(pair.second);
            pair.second->On_Collision_Exit(pair.first);
        }
    }

    // 교체
    m_PrevCollisionPairs = m_CurrCollisionPairs;
    m_ColliderContainer.clear(); // 프레임마다 초기화
}

void CCollisionMgr::LateUpdate_Collision(_float& dt)
{
}

void CCollisionMgr::Render_Collider(LPDIRECT3DDEVICE9 pDevice)
{
    for (CCollider* collider : m_ColliderContainer)
    {
        if (collider && collider->Get_ComponentActive())
        {
            collider->Render_Debug(pDevice);
        }
    }
}

void CCollisionMgr::Register_Collider(CCollider* collider)
{
    if (collider)
        m_ColliderContainer.push_back(collider);
}

bool CCollisionMgr::IsAABBOverlap(const AABB& a, const AABB& b)
{
    return (a.vMin.x <= b.vMax.x && a.vMax.x >= b.vMin.x) &&
        (a.vMin.y <= b.vMax.y && a.vMax.y >= b.vMin.y) &&
        (a.vMin.z <= b.vMax.z && a.vMax.z >= b.vMin.z);
}

void CCollisionMgr::Free()
{
}
