#include "Engine_Define.h"
#include "CPhysicsMgr.h"
#include "CRigidBody.h"
#include "CTransform.h"
#include "CGameObject.h"

IMPLEMENT_SINGLETON(CPhysicsMgr)

CPhysicsMgr::CPhysicsMgr()
{
}

CPhysicsMgr::~CPhysicsMgr()
{
}

HRESULT CPhysicsMgr::Ready_Physics()
{
	return S_OK;
}

void CPhysicsMgr::Update_Physics(_float& dt)
{
	if (!m_bSimulate) return;

	for (CRigidBody* rb : m_RigidContainer) {
		if (!rb->Get_ComponentActive() || rb->Get_Type() == CRigidBody::RigidbodyType::Static)
			continue;

		CTransform* transform = rb->m_pOwner->Get_Component<CTransform>();
		_vec3 deltaPos = (m_Gravity * rb->Get_Mass()) * (0.5f * dt * dt);
		transform->Add_Pos(deltaPos );
	}


	m_RigidContainer.clear();
}

void CPhysicsMgr::LateUpdate_Physics(_float& dt)
{
	if (!m_bSimulate) return;
}

void CPhysicsMgr::Render_Physics()
{
	ImGui::Checkbox("##ActivePhysics", &m_bSimulate); ImGui::SameLine();
}

void CPhysicsMgr::Register_Physics(CRigidBody* rigid)
{
	if (!rigid || !rigid->Get_ComponentActive()) return;

	m_RigidContainer.push_back(rigid);
}

void CPhysicsMgr::Free()
{
}
