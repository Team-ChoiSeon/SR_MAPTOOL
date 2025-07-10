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

void CRigidBody::Update_Component( _float& fDeltaTime)
{
    if (m_pTransform == nullptr)
    {
        m_pTransform = m_pOwner->Get_Component<CTransform>();
        return;
    }
    if (!m_bSimulate) return;

    // 중력 적용
    if (m_bGravity && !m_bGround)
    {
        m_vGforce = _vec3(0.f, -9.8f * m_fGravity, 0.f) * m_fMass;
    }
    else
    {
        m_vGforce = _vec3(0.f, 0.f, 0.f);
    }

    // 외력 + 중력
    _vec3 totalForce = m_vEforce + m_vGforce;

    // 마찰력 : 지면 위에서만
    if (m_bGround)
    {
        _vec3 friction = -m_vVel;
        friction.y = 0.f; // y축 마찰 무시 (점프/낙하 방해 방지)
        friction *= m_fFric;
        totalForce += friction;
    }


    if (m_fMass == 0.f) return;
    // F = m * a  =>  a = F / m
    m_vAcc = totalForce / m_fMass;
    // 속도 업데이트
    m_vVel += m_vAcc * fDeltaTime;

    // 지면에서 미세한 움직임 컷팅
    if (m_bGround)
    {
        if (D3DXVec3LengthSq(&m_vVel) < 0.001f)
            m_vVel = _vec3(0.f, 0.f, 0.f);

        if (D3DXVec3LengthSq(&m_vAcc) < 0.01f)
            m_vAcc = _vec3(0.f, 0.f, 0.f);
    }

    // 낙하 처리 : 탄성 적용
    if (m_bGround && m_vVel.y < 0.f)
    {
        m_vVel.y *= -m_fBnc;

        if (fabs(m_vVel.y) < 0.05f)
        {
            m_vVel.y = 0.f;
            m_vAcc.y = 0.f;
        }
        else
        {
            // 여기서 튕겨 오르면 지면에서 떨어진 상태이므로 false 처리
            m_bGround = false;
        }
    }

    // 위치 업데이트
    _vec3 vPos = m_pTransform->Get_Pos();
    vPos += m_vVel * fDeltaTime;
    m_pTransform->Set_Pos(vPos);

    // 회전 반영
    m_vAAcc = { m_vTorque.x / m_fInertia.x ,m_vTorque.y / m_fInertia.y ,m_vTorque.z / m_fInertia.z };
    m_vAVel += m_vAAcc * fDeltaTime;
    if (D3DXVec3LengthSq(&m_vAVel) > 0.f)
    {
        _vec3 axis = m_vAVel;
        float angle = D3DXVec3Length(&axis) * fDeltaTime;

        D3DXVec3Normalize(&axis, &axis);
        m_pTransform->Rotate_Axis(axis, angle);
    }
    // 감쇠 및 초기화
    m_vAVel *= 0.995f;
    m_vTorque = _vec3(0.f, 0.f, 0.f);


    m_vEforce = _vec3(0.f, 0.f, 0.f);
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

void CRigidBody::Serialize(json& outJson) const
{
    outJson["m_bGravity"] = m_bUseGravity;

    outJson["Mass"] = m_fMass;
    outJson["Friction"] = m_fFric;
    outJson["Bounciness"] = m_fBnc;

    outJson["Velocity"] = { m_vVel.x, m_vVel.y, m_vVel.z };
    outJson["Accelator"] = { m_vAcc.x, m_vAcc.y, m_vAcc.z };
    outJson["AngleVelocity"] = { m_vAVel.x, m_vAVel.y, m_vAVel.z };
    outJson["AngleAccel"] = { m_vAAcc.x, m_vAAcc.y, m_vAAcc.z };

    outJson["ExternalForce"] = { m_vEforce.x, m_vEforce.y, m_vEforce.z };
    outJson["Torque"] = { m_vTorque.x, m_vTorque.y, m_vTorque.z };
    outJson["Inertia"] = { m_fInertia.x, m_fInertia.y, m_fInertia.z };
}

void CRigidBody::Deserialize(const json& inJson)
{
    if (inJson.contains("m_bGravity"))
        m_bUseGravity = inJson["m_bGravity"].get<bool>();

    if (inJson.contains("Mass"))
        m_fMass = inJson["Mass"].get<float>();

    if (inJson.contains("Friction"))
        m_fFric = inJson["Friction"].get<float>();

    if (inJson.contains("Bounciness"))
        m_fBnc = inJson["Bounciness"].get<float>();

    if (inJson.contains("Velocity")) {
        m_vVel.x = inJson["Velocity"][0].get<float>();
        m_vVel.y = inJson["Velocity"][1].get<float>();
        m_vVel.z = inJson["Velocity"][2].get<float>();
    }

    if (inJson.contains("Accelator")) {
        m_vAcc.x = inJson["Accelator"][0].get<float>();
        m_vAcc.y = inJson["Accelator"][1].get<float>();
        m_vAcc.z = inJson["Accelator"][2].get<float>();
    }

    if (inJson.contains("AngleVelocity")) {
        m_vAVel.x = inJson["AngleVelocity"][0].get<float>();
        m_vAVel.y = inJson["AngleVelocity"][1].get<float>();
        m_vAVel.z = inJson["AngleVelocity"][2].get<float>();
    }

    if (inJson.contains("AngleAccel")) {
        m_vAAcc.x = inJson["AngleAccel"][0].get<float>();
        m_vAAcc.y = inJson["AngleAccel"][1].get<float>();
        m_vAAcc.z = inJson["AngleAccel"][2].get<float>();
    }

    if (inJson.contains("ExternalForce")) {
        m_vEforce.x = inJson["ExternalForce"][0].get<float>();
        m_vEforce.y = inJson["ExternalForce"][1].get<float>();
        m_vEforce.z = inJson["ExternalForce"][2].get<float>();
    }

    if (inJson.contains("Torque")) {
        m_vTorque.x = inJson["Torque"][0].get<float>();
        m_vTorque.y = inJson["Torque"][1].get<float>();
        m_vTorque.z = inJson["Torque"][2].get<float>();
    }

    if (inJson.contains("Inertia")) {
        m_fInertia.x = inJson["Inertia"][0].get<float>();
        m_fInertia.y = inJson["Inertia"][1].get<float>();
        m_fInertia.z = inJson["Inertia"][2].get<float>();
    }
}

string CRigidBody::Get_ComponentName() const
{
	return "CRigidBody";
}

void CRigidBody::Free()
{
}
