#include "Engine_Define.h"
#include "CPhysicsMgr.h"
#include "CRigidBody.h"
#include "CTransform.h"
#include "CGameObject.h"

IMPLEMENT_SINGLETON(CPhysicsMgr)

CPhysicsMgr::CPhysicsMgr()
	:m_bSimulate(false)
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
	for (CRigidBody* rb : m_RigidContainer) {
		rb->Set_Simulate(m_bSimulate);
	}
	m_RigidContainer.clear();

}

void CPhysicsMgr::LateUpdate_Physics(_float& dt)
{
}

void CPhysicsMgr::Render_Physics()
{
	//ImGui::Checkbox("##ActivePhysics", &m_bSimulate); ImGui::SameLine();
}

void CPhysicsMgr::Register_Physics(CRigidBody* rigid)
{
	if (!rigid || !rigid->Get_ComponentActive()) return;

	m_RigidContainer.push_back(rigid);
}

void CPhysicsMgr::Free()
{
}
