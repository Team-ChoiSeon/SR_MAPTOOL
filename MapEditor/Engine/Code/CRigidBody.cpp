#include "Engine_Define.h"
#include "CRigidBody.h"
#include "CCollider.h"
#include "CPhysicsMgr.h"
#include "CGameObject.h"
#include "CTransform.h"

CRigidBody::CRigidBody()
{
}

CRigidBody::~CRigidBody()
{
}

CRigidBody* CRigidBody::Create()
{
	CRigidBody* instance = new CRigidBody;

	if (FAILED(instance->Ready_Component())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CRigidBody::Ready_Component()
{
	return S_OK;
}

void CRigidBody::Update_Component(_float& dt)
{
}

void CRigidBody::LateUpdate_Component(_float& dt)
{
	CPhysicsMgr::GetInstance()->Register_Physics(this);
}

void CRigidBody::Render_Panel(ImVec2 size)
{

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);

	ImGui::Checkbox("##ActiveRigid", &m_bActive); ImGui::SameLine();

	if (ImGui::CollapsingHeader("RigidBody"))
	{
		Delete_ComponentPane();
		int tmpOption = static_cast<int>(m_eType);
		ImGui::RadioButton("static", &tmpOption, 1);
		ImGui::RadioButton("kinematic", &tmpOption, 2);
		ImGui::RadioButton("dynamic", &tmpOption, 3);
		m_eType = static_cast<RigidbodyType>(tmpOption);

        ImGui::Checkbox("Use Gravity", &m_bUseGravity);
        ImGui::DragFloat("Mass", &m_fMass, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Friction", &m_fFric, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Bounciness", &m_fBnc, 0.01f, 0.0f, 1.0f);

        ImGui::DragFloat3("Velocity", (float*)&m_vVel, 0.1f);
        ImGui::DragFloat3("Acceleration", (float*)&m_vAcc, 0.1f);
        ImGui::DragFloat3("Angular Velocity", (float*)&m_vAVel, 0.1f);
        ImGui::DragFloat3("External Force", (float*)&m_vEforce, 0.1f);
	}
}

CComponent* CRigidBody::Clone() const
{
	return nullptr;
}

void CRigidBody::Resolve_PhysicsCollision(CCollider* self, CCollider* other) //밀어내기
{
    if (m_eType == Kinematic || m_eType == Static)
        return;

    // other가 dynamic이든 static이든 상관없이 반응해야 함
    CRigidBody* otherRB = other->m_pOwner->Get_Component<CRigidBody>();

    const AABB& myBox = self->Get_AABBBox();
    const AABB& otherBox = other->Get_AABBBox();

    // 1. penetration 방향 계산 (심플하게 중심 좌표로)
    _vec3 dir = self->m_pOwner->Get_Component<CTransform>()->Get_Pos()
        - other->m_pOwner->Get_Component<CTransform>()->Get_Pos();

    // 2. penetration 깊이 계산 (간단한 min/max 방식)
    _vec3 penetration;
    penetration.x =min(myBox.vMax.x - otherBox.vMin.x, otherBox.vMax.x - myBox.vMin.x);
    penetration.y =min(myBox.vMax.y - otherBox.vMin.y, otherBox.vMax.y - myBox.vMin.y);
    penetration.z =min(myBox.vMax.z - otherBox.vMin.z, otherBox.vMax.z - myBox.vMin.z);

    // 가장 작은 축으로만 이동 (Axis of Minimum Penetration)
    if (penetration.x < penetration.y && penetration.x < penetration.z)
        dir = _vec3(dir.x > 0 ? penetration.x : -penetration.x, 0, 0);
    else if (penetration.y < penetration.z)
        dir = _vec3(0, dir.y > 0 ? penetration.y : -penetration.y, 0);
    else
        dir = _vec3(0, 0, dir.z > 0 ? penetration.z : -penetration.z);

    // 3. 위치 보정 (밀어내기)
    self->m_pOwner->Get_Component<CTransform>()->Add_Pos(dir);

    // 4. 반사 혹은 속도 조정 (간단하게 정지시켜보기)
    // 3. 반발력 + 마찰력 계산
    _vec3 otherVel = otherRB ? otherRB->m_vVel : _vec3(0, 0, 0);
    _vec3 relativeVel = m_vVel - otherVel; //속도 차이 

    _vec3 normal;
    D3DXVec3Normalize(&normal ,&dir);

    float relVelN = D3DXVec3Dot(&relativeVel, &normal);
    if (relVelN > 0) return;

    /*탄성 공식*/
    float impulseMag = -(1 + m_fBnc) * relVelN; // 상대 속도의 반사량
    impulseMag /= (1 / m_fMass + (otherRB ? 1 / otherRB->m_fMass : 0));
    //내 질량분의 1 + 상대 질량 분의 1 을 상대 속도  반사량에 나눠줌. 


    _vec3 impulse = impulseMag * normal;
    m_vVel += impulse / m_fMass; //탄성 더해주기

    if (otherRB && otherRB->Get_Type() == Dynamic)
        otherVel -= impulse / otherRB->m_fMass; //상대는 반대쪽으로 밀어주기

    // 마찰력 공식 //
    _vec3 tangent = relativeVel - normal * relVelN;
    if (D3DXVec3LengthSq(&tangent) > 0.0001f) //0이 아니면
        D3DXVec3Normalize(&tangent,&tangent);

    float fImpulse = -D3DXVec3Dot(&relativeVel, &tangent); //물리량

    fImpulse *= m_fFric;
    fImpulse /= (1 / m_fMass + (otherRB ? 1 / otherRB->m_fMass : 0));

    _vec3 frictionImpulse = fImpulse * tangent;
    m_vVel += frictionImpulse / m_fMass;

    if (otherRB && otherRB->Get_Type() == Dynamic)
        otherVel -= frictionImpulse / otherRB->m_fMass;
   
}

void CRigidBody::Serialize(json& outJson) const
{
}

void CRigidBody::Deserialize(const json& inJson)
{
}

string CRigidBody::Get_ComponentName() const
{
	return "CRigidBody";
}

void CRigidBody::Free()
{
}
