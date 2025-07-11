#include "Engine_Define.h"
#include "CCollider.h"
#include "CCollisionMgr.h"
#include "CRigidBody.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CGraphicDev.h"
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
	//instance->Ready_DebugBox(CGraphicDev::GetInstance()->Get_GraphicDev());
	return instance;
}

HRESULT CCollider::Ready_Component()
{
	return S_OK;
}

void CCollider::Update_Component(_float& dt)
{
	//if (m_eState == ColliderState::ENTER) m_eState = ColliderState::STAY;
	//else if (m_eState == ColliderState::EXIT) m_eState = ColliderState::NONE;
	//
	//if (!m_pTransform) {
	//	m_pTransform = m_pOwner->Get_Component<CTransform>();
	//};
	//if (!m_pRigid) {
	//	m_pRigid = m_pOwner->Get_Component<CRigidBody>();
	//};
	//
	//const _matrix* pWorld = &m_pTransform->Get_WorldMatrix();
	//
	//AABB aabb;
	//aabb.vMin = m_tAABB.vMin + m_tAABBOff.vMin;
	//aabb.vMax = m_tAABB.vMax + m_tAABBOff.vMax;
	//
	//// 8개의 로컬 꼭짓점 → 월드 변환
	//_vec3 corners[8] = {
	//	{aabb.vMin.x, aabb.vMin.y, aabb.vMin.z}, {aabb.vMax.x, aabb.vMin.y, aabb.vMin.z},
	//	{aabb.vMax.x, aabb.vMax.y, aabb.vMin.z}, {aabb.vMin.x, aabb.vMax.y, aabb.vMin.z},
	//	{aabb.vMin.x, aabb.vMin.y, aabb.vMax.z}, {aabb.vMax.x, aabb.vMin.y, aabb.vMax.z},
	//	{aabb.vMax.x, aabb.vMax.y, aabb.vMax.z}, {aabb.vMin.x, aabb.vMax.y, aabb.vMax.z}
	//};
	//
	//for (int i = 0; i < 8; ++i)
	//	D3DXVec3TransformCoord(&corners[i], &corners[i], pWorld);
	//
	//// AABB 월드 좌표 재계산
	//m_tAABBWorld.vMin = m_tAABBWorld.vMax = corners[0];
	//for (int i = 1; i < 8; ++i)
	//{
	//	m_tAABBWorld.vMin.x = min(m_tAABBWorld.vMin.x, corners[i].x);
	//	m_tAABBWorld.vMin.y = min(m_tAABBWorld.vMin.y, corners[i].y);
	//	m_tAABBWorld.vMin.z = min(m_tAABBWorld.vMin.z, corners[i].z);
	//	m_tAABBWorld.vMax.x = max(m_tAABBWorld.vMax.x, corners[i].x);
	//	m_tAABBWorld.vMax.y = max(m_tAABBWorld.vMax.y, corners[i].y);
	//	m_tAABBWorld.vMax.z = max(m_tAABBWorld.vMax.z, corners[i].z);
	//}
	//
	//// Bounding 정보 계산
	//if (m_tBound.eType == BoundingType::OBB)
	//{
	//	// OBB 계산 (축 방향 반지름 계산 및 변환)
	//	_vec3 vHalf = (aabb.vMax - aabb.vMin) * 0.5f;
	//	_vec3 vScale = {
	//		D3DXVec3Length((_vec3*)&pWorld->_11),
	//		D3DXVec3Length((_vec3*)&pWorld->_21),
	//		D3DXVec3Length((_vec3*)&pWorld->_31)
	//	};
	//	m_tBound.vHalf = _vec3(vHalf.x * vScale.x, vHalf.y * vScale.y, vHalf.z * vScale.z);
	//	m_tBound.Calc_Transform(*pWorld);
	//}
	//else
	//{
	//	// AABB이지만 OBB 계산기(Calc_Push_OBB)를 위해 꼭짓점 저장
	//	m_tBound.vCorners.clear();
	//	m_tBound.vCorners.reserve(8);
	//	for (int i = 0; i < 8; ++i)
	//		m_tBound.vCorners.push_back(corners[i]);
	//
	//	// 중심 위치 갱신
	//	m_tBound.vCenter = (m_tAABBWorld.vMin + m_tAABBWorld.vMax) * 0.5f;
	//
	//	// 단위 벡터 설정 (축 고정)
	//	m_tBound.vAxisX = _vec3(1.f, 0.f, 0.f);
	//	m_tBound.vAxisY = _vec3(0.f, 1.f, 0.f);
	//	m_tBound.vAxisZ = _vec3(0.f, 0.f, 1.f);
	//
	//	// 반지름 길이 갱신
	//	m_tBound.vHalf = (m_tAABBWorld.vMax - m_tAABBWorld.vMin) * 0.5f;
	//}
	//
	//// 충돌 매니저 등록
	//CCollisionMgr::GetInstance()->Register_Collider(this);
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

		// Collider Type Combo
		const char* typeItems[] = { "ACTIVE", "PASSIVE", "TRIGGER" };
		int currentType = static_cast<int>(m_eType);
		if (ImGui::Combo("Type", &currentType, typeItems, IM_ARRAYSIZE(typeItems)))
			m_eType = static_cast<ColliderType>(currentType);

		// Collider Tag Combo
		const char* tagItems[] = { "NONE", "GROUND", "PLAYER", "ENEMY", "PLATFORM" };
		int currentTag = static_cast<int>(m_eTag);
		if (ImGui::Combo("Tag", &currentTag, tagItems, IM_ARRAYSIZE(tagItems)))
			m_eTag = static_cast<ColliderTag>(currentTag);

		// Collider State (읽기 전용)
		const char* stateItems[] = { "NONE", "ENTER", "STAY", "EXIT" };
		ImGui::Text("State: %s", stateItems[static_cast<int>(m_eState)]);
	}
}

void CCollider::Ready_DebugBox(LPDIRECT3DDEVICE9 pDevice)
{
	if (!pDevice) return;

	// 8 정점 (정육면체 코너)
	VTXCOL vertices[8] = {
		{{-1, -1, -1}, D3DCOLOR_XRGB(0,255,0)},
		{{ 1, -1, -1}, D3DCOLOR_XRGB(0,255,0)},
		{{ 1,  1, -1}, D3DCOLOR_XRGB(0,255,0)},
		{{-1,  1, -1}, D3DCOLOR_XRGB(0,255,0)},
		{{-1, -1,  1}, D3DCOLOR_XRGB(0,255,0)},
		{{ 1, -1,  1}, D3DCOLOR_XRGB(0,255,0)},
		{{ 1,  1,  1}, D3DCOLOR_XRGB(0,255,0)},
		{{-1,  1,  1}, D3DCOLOR_XRGB(0,255,0)},
	};

	WORD indices[24] = {
		0,1, 1,2, 2,3, 3,0, // 아래
		4,5, 5,6, 6,7, 7,4, // 위
		0,4, 1,5, 2,6, 3,7  // 옆
	};

	// VB
	pDevice->CreateVertexBuffer(sizeof(vertices), 0, FVF_COL, D3DPOOL_MANAGED, &m_tDebug.pVB, nullptr);
	void* pVerts = nullptr;
	m_tDebug.pVB->Lock(0, 0, &pVerts, 0);
	memcpy(pVerts, vertices, sizeof(vertices));
	m_tDebug.pVB->Unlock();

	// IB
	pDevice->CreateIndexBuffer(sizeof(indices), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_tDebug.pIB, nullptr);
	void* pIndices = nullptr;
	m_tDebug.pIB->Lock(0, 0, &pIndices, 0);
	memcpy(pIndices, indices, sizeof(indices));
	m_tDebug.pIB->Unlock();
}

void CCollider::Render_Debug(LPDIRECT3DDEVICE9 pDevice)
{
	//if (!pDevice || !m_bActive || !m_tDebug.pVB || !m_tDebug.pIB)
	//	return;
	//
	//// 현재 AABB 박스를 기준으로 월드 행렬 구성
	//const AABB& box = Get_AABBBox();
	//D3DXVECTOR3 center = (box.vMin + box.vMax) * 0.5f;
	//D3DXVECTOR3 size = (box.vMax - box.vMin) * 1.15f; // 15% 크게
	//
	//D3DXMATRIX matScale, matTrans, matWorld;
	//D3DXMatrixScaling(&matScale, size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
	//D3DXMatrixTranslation(&matTrans, center.x, center.y, center.z);
	//matWorld = matScale * matTrans;
	//
	//pDevice->SetTransform(D3DTS_WORLD, &matWorld);
	//
	//// 조명 OFF, 색상 및 버퍼 설정
	//pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	//pDevice->SetFVF(FVF_COL);
	//pDevice->SetStreamSource(0, m_tDebug.pVB, 0, sizeof(VTXCOL));
	//pDevice->SetIndices(m_tDebug.pIB);
	//pDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 8, 0, 12);
	//pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}



CComponent* CCollider::Clone() const
{
	return nullptr;
}

void CCollider::On_Collision_Enter(CCollider* pOther)
{
	ColliderType oType = pOther->Get_ColType();
	if (m_eType == ColliderType::TRIGGER || oType == ColliderType::TRIGGER)
		return;

	_vec3 push(0.f, 0.f, 0.f);
	bool pushed = false;

	if (m_eType == ColliderType::ACTIVE && oType == ColliderType::PASSIVE)
	{
		if (m_tBound.eType == BoundingType::AABB && pOther->Get_Bound().eType == BoundingType::AABB)
			pushed = Calc_Push_AABB(Get_AABBW(), pOther->Get_AABBW(), push);
		else
			pushed = Calc_Push_OBB(Get_Bound(), pOther->Get_Bound(), push);

		if (pushed)
		{
			if (auto pTransform = m_pOwner->Get_Component<CTransform>())
				pTransform->Set_Pos(pTransform->Get_Pos() + push);
			Handle_Ground(pOther, push);
		}
	}
	else if (m_eType == ColliderType::ACTIVE && oType == ColliderType::ACTIVE)
	{
		pushed = Calc_Push_AABB(Get_AABBW(), pOther->Get_AABBW(), push);
		if (!pushed || !m_pRigid || !pOther->m_pRigid)
			return;

		if (auto pTransform = m_pOwner->Get_Component<CTransform>())
			pTransform->Set_Pos(pTransform->Get_Pos() + push);

		float m1 = m_pRigid->Get_Mass(), m2 = pOther->m_pRigid->Get_Mass();
		float total = m1 + m2;
		m_pRigid->Add_Force(push * (m2 / total) * 1000.f);
		pOther->m_pRigid->Add_Force(-push * (m1 / total) * 1000.f);
	}

	if (m_eState == ColliderState::NONE || m_eState == ColliderState::EXIT)
		m_eState = ColliderState::ENTER;
}

void CCollider::On_Collision_Stay(CCollider* pOther)
{
	ColliderType oType = pOther->Get_ColType();
	if (m_eType == ColliderType::ACTIVE && (oType == ColliderType::PASSIVE || oType == ColliderType::ACTIVE))
	{
		_vec3 push(0.f, 0.f, 0.f);
		bool pushed = false;

		if (m_tBound.eType == BoundingType::AABB && pOther->Get_Bound().eType == BoundingType::AABB)
			pushed = Calc_Push_AABB(Get_AABBW(), pOther->Get_AABBW(), push);
		else
			pushed = Calc_Push_OBB(Get_Bound(), pOther->Get_Bound(), push);

		if (pushed)
		{
			if (auto pTransform = m_pOwner->Get_Component<CTransform>())
				pTransform->Set_Pos(pTransform->Get_Pos() + push);

			if (oType == ColliderType::ACTIVE && m_pRigid && pOther->m_pRigid)
			{
				float m1 = m_pRigid->Get_Mass(), m2 = pOther->m_pRigid->Get_Mass();
				float total = m1 + m2;
				m_pRigid->Add_Force(push * (m2 / total) * 1000.f);
				pOther->m_pRigid->Add_Force(-push * (m1 / total) * 1000.f);
			}

			Handle_Ground(pOther, push);
		}
	}

	m_eState = (m_eState == ColliderState::NONE) ? ColliderState::ENTER : ColliderState::STAY;
}

void CCollider::On_Collision_Exit(CCollider* pOther)
{
	if (m_eType == ColliderType::ACTIVE && pOther->Get_ColTag() == ColliderTag::GROUND)
	{
		if (m_pRigid)
			m_pRigid->Set_OnGround(false);
	}

	m_eState = ColliderState::EXIT;
}

bool CCollider::Calc_Push_AABB(const AABB& a, const AABB& b, _vec3& push)
{
	_vec3 vMyCenter = (a.vMin + a.vMax) * 0.5f;
	_vec3 vOtherCenter = (b.vMin + b.vMax) * 0.5f;

	_vec3 overlap;
	overlap.x = min(a.vMax.x, b.vMax.x) - max(a.vMin.x, b.vMin.x);
	overlap.y = min(a.vMax.y, b.vMax.y) - max(a.vMin.y, b.vMin.y);
	overlap.z = min(a.vMax.z, b.vMax.z) - max(a.vMin.z, b.vMin.z);


	if (overlap.x <= overlap.y && overlap.x <= overlap.z)
		push.x = (vMyCenter.x > vOtherCenter.x) ? overlap.x : -overlap.x;
	else if (overlap.y <= overlap.z)
		push.y = (vMyCenter.y > vOtherCenter.y) ? overlap.y : -overlap.y;
	else
		push.z = (vMyCenter.z > vOtherCenter.z) ? overlap.z : -overlap.z;

	return true;
}

bool CCollider::Calc_Push_OBB(const BoundInfo& a, const BoundInfo& b, _vec3& push)
{
	vector<_vec3> vAxis;

	vAxis.push_back(a.vAxisX);
	vAxis.push_back(a.vAxisY);
	vAxis.push_back(a.vAxisZ);

	vAxis.push_back(b.vAxisX);
	vAxis.push_back(b.vAxisY);
	vAxis.push_back(b.vAxisZ);

	_vec3 cross;
	for (int i = 0; i < 3; ++i)
	{
		_vec3 aAxis = (&a.vAxisX)[i];
		for (int j = 0; j < 3; ++j)
		{
			_vec3 bAxis = (&b.vAxisX)[j];
			D3DXVec3Cross(&cross, &aAxis, &bAxis);
			if (D3DXVec3LengthSq(&cross) > 0.0001f) // 유효한 축만
			{
				D3DXVec3Normalize(&cross, &cross);
				vAxis.push_back(cross);
			}
		}
	}

	float minOverlap = FLT_MAX;
	_vec3 mtvAxis = { 0.f, 0.f, 0.f };

	for (const _vec3& axis : vAxis)
	{
		float minA = 0.f, maxA = 0.f;
		float minB = 0.f, maxB = 0.f;

		// A 투영
		{
			float dot = D3DXVec3Dot(&a.vCorners[0], &axis);
			minA = maxA = dot;
			for (int i = 1; i < 8; ++i)
			{
				dot = D3DXVec3Dot(&a.vCorners[i], &axis);
				if (dot < minA) minA = dot;
				if (dot > maxA) maxA = dot;
			}
		}

		// B 투영
		{
			float dot = D3DXVec3Dot(&b.vCorners[0], &axis);
			minB = maxB = dot;
			for (int i = 1; i < 8; ++i)
			{
				dot = D3DXVec3Dot(&b.vCorners[i], &axis);
				if (dot < minB) minB = dot;
				if (dot > maxB) maxB = dot;
			}
		}

		float overlap = min(maxA, maxB) - max(minA, minB);
		if (overlap <= 0.f)
			return false; // 분리 축 발견 → 충돌 아님

		if (overlap < minOverlap)
		{
			minOverlap = overlap;
			mtvAxis = axis;

			// 중심 기준 방향 보정
			_vec3 dir = a.vCenter - b.vCenter;
			if (D3DXVec3Dot(&dir, &axis) < 0.f)
				mtvAxis = -mtvAxis;
		}
	}

	push = mtvAxis * minOverlap;
	return true;
}

void CCollider::Handle_Ground(CCollider* pOther, const _vec3& push)
{
	if (!m_pRigid || pOther->Get_ColTag() != ColliderTag::GROUND)
		return;

	// 위쪽에서 눌렀는지 확인
	if (push.y > 0.f && push.y > abs(push.x) && push.y > abs(push.z))
	{
		m_pRigid->Set_OnGround(true);
	}
}

void CCollider::Serialize(json& outJson) const
{
	outJson["ColType"] = static_cast<int>(m_eType);
	outJson["ColTag"] = static_cast<int>(m_eTag);
	outJson["BoundType"] = static_cast<int>(m_tBound.eType);
}

void CCollider::Deserialize(const json& inJson)
{
	if (inJson.contains("ColType"))
		m_eType = static_cast<ColliderType>(inJson["ColType"].get<int>());

	if (inJson.contains("ColTag"))
		m_eTag = static_cast<ColliderTag>(inJson["ColTag"].get<int>());

	if (inJson.contains("BoundType"))
		m_tBound.eType = static_cast<BoundingType>(inJson["BoundType"].get<int>());

}

string CCollider::Get_ComponentName() const
{
	return "CCollider";
}

void CCollider::Free()
{
}