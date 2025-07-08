#include "Engine_Define.h"
#include "CCollider.h"
#include "CCollisionMgr.h"
#include "CRigidBody.h"
#include "CGameObject.h"

CCollider::CCollider()
{
}

CCollider::~CCollider()
{
}

CCollider* CCollider::Create()
{
	CCollider* instance = new CCollider;

	if (FAILED(instance->Ready_Component())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CCollider::Ready_Component()
{
	return S_OK;
}

void CCollider::Update_Component(_float& dt)
{
}

void CCollider::LateUpdate_Component(_float& dt)
{
	if (m_bActive)
		CCollisionMgr::GetInstance()->Register_Collider(this);
}

void CCollider::Render_Panel(ImVec2 size)
{

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);

	ImGui::Checkbox("##ActiveCollider", &m_bActive); ImGui::SameLine();

	if (ImGui::CollapsingHeader("Collider"))
	{
		Delete_ComponentPane();
		//ImGui::SliderFloat3("OffSetLeft",)
	}
}
void CCollider::Render_Debug(LPDIRECT3DDEVICE9 pDevice)
{

    if (!pDevice || !m_bActive) return;

    const AABB& box = Get_AABBBox(); // 최소/최대 좌표
    D3DXVECTOR3 min = box.vMin;
    D3DXVECTOR3 max = box.vMax;

    
    VTXCOL vertices[24];
    DWORD green = D3DCOLOR_XRGB(0, 255, 0);

    // 12 라인 구성 (정육면체 모서리 12개)
    D3DXVECTOR3 corners[8] = {
        {min.x, min.y, min.z},          // 0
        {max.x, min.y, min.z},          // 1
        {max.x, max.y, min.z},       // 2
        {min.x, max.y, min.z},       // 3
        {min.x, min.y, max.z},       // 4
        {max.x, min.y, max.z},       // 5
        {max.x, max.y, max.z},      // 6
        {min.x, max.y, max.z}       // 7
    };

    // 모서리 라인 (12개)
    int idx = 0;
    auto AddEdge = [&](int a, int b) {
        vertices[idx++] = { corners[a], green };
        vertices[idx++] = { corners[b], green };
        };

    AddEdge(0, 1); AddEdge(1, 2); AddEdge(2, 3); AddEdge(3, 0); // 아래
    AddEdge(4, 5); AddEdge(5, 6); AddEdge(6, 7); AddEdge(7, 4); // 위
    AddEdge(0, 4); AddEdge(1, 5); AddEdge(2, 6); AddEdge(3, 7); // 사이드

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDevice->SetFVF(FVF_COL);
    pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 12, vertices, sizeof(VTXCOL));
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}


CComponent* CCollider::Clone() const
{
	return nullptr;
}

void CCollider::On_CollisionEnter(CCollider* other)
{
	// 처음 충돌한 경우
	if (CRigidBody* rigid = m_pOwner->Get_Component<CRigidBody>())
	{
		rigid->Resolve_PhysicsCollision(this, other);
	}
}

void CCollider::On_CollisionStay(CCollider* other)
{
	// 계속 충돌 중
}

void CCollider::On_CollisionExit(CCollider* other)
{
	// 충돌 종료
}

void CCollider::Serialize(json& outJson) const
{
}

void CCollider::Deserialize(const json& inJson)
{
}

string CCollider::Get_ComponentName() const
{
	return "CCollider";
}

void CCollider::Free()
{
}