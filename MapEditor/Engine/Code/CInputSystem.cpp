#include "Engine_Define.h"
#include "CInputSystem.h"
#include "CInputMgr.h"
#include "CTransform.h"
#include "CSceneMgr.h"
#include "CGameObject.h"


CInputSystem::CInputSystem()
	:m_fMoveSpeed(0.f), m_fRotateSpeed(0.f)
{
}

CInputSystem::~CInputSystem()
{
}

CInputSystem* CInputSystem::Create()
{
	CInputSystem* instance = new CInputSystem;

	if (FAILED(instance->Ready_Component())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CInputSystem::Ready_Component()
{
	return S_OK;
}

void CInputSystem::Update_Component(_float& dt)
{
	if (CSceneMgr::GetInstance()->Get_SelectedObject() != m_pOwner) return;
	m_bMove = false; // <- 초기화 먼저
	if (CInputMgr::GetInstance()->Key_Down(DIK_UP)) {
		_vec3 dir = { 0,0,1 };
		m_pOwner->Get_Component<CTransform>()->Add_Pos(dir *dt*m_fMoveSpeed);
		m_bMove = true;
	}
	if (CInputMgr::GetInstance()->Key_Down(DIK_DOWN)) {
		_vec3 dir = { 0,0,-1 };
		m_pOwner->Get_Component<CTransform>()->Add_Pos(dir * dt * m_fMoveSpeed);
		m_bMove = true;
	}
	if (CInputMgr::GetInstance()->Key_Down(DIK_RIGHT)) {
		_vec3 dir = { 1,0,0 };
		m_pOwner->Get_Component<CTransform>()->Add_Pos(dir * dt * m_fMoveSpeed);
		m_bMove = true;
	}	
	if (CInputMgr::GetInstance()->Key_Down(DIK_LEFT)) {
		_vec3 dir = { -1,0,0 };
		m_pOwner->Get_Component<CTransform>()->Add_Pos(dir * dt * m_fMoveSpeed);
		m_bMove = true;
	}

	if (CInputMgr::GetInstance()->Key_Down(DIK_Z)) {
		_vec3 dir = { 0,0,1 };
		m_pOwner->Get_Component<CTransform>()->Add_Rotate(dir * dt * m_fRotateSpeed);
		m_bMove = true;
	}

	if (CInputMgr::GetInstance()->Key_Down(DIK_X)) {
		_vec3 dir = { 1,0,0};
		m_pOwner->Get_Component<CTransform>()->Add_Rotate(dir * dt * m_fRotateSpeed);
		m_bMove = true;
	}
	if (CInputMgr::GetInstance()->Key_Down(DIK_C)) {
		_vec3 dir = { 0,1,0 };
		m_pOwner->Get_Component<CTransform>()->Add_Rotate(dir * dt * m_fRotateSpeed);
		m_bMove = true;
	}
}

void CInputSystem::LateUpdate_Component(_float& dt)
{
}

void CInputSystem::Render_Panel(ImVec2 size)
{
	ImGui::SetNextItemOpen(false, ImGuiCond_Once);

	ImGui::Checkbox("##ActivInput", &m_bActive); ImGui::SameLine();
	ImGui::Text("Valid Input");

	ImGui::SliderFloat("Move Speed", &m_fMoveSpeed, 0.0f, 50.0f, "%.1f");
	ImGui::SliderFloat("Rotate Speed", &m_fRotateSpeed, 0.0f, 360.0f, "%.1f");
}


CComponent* CInputSystem::Clone() const
{
	return nullptr;
}

void CInputSystem::Serialize(json& outJson) const
{
}

void CInputSystem::Deserialize(const json& inJson)
{
}

string CInputSystem::Get_ComponentName() const
{
	return "CInputSystem";
}

void CInputSystem::Free()
{
}