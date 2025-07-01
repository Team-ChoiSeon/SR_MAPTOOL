#include "pch.h"
#include "Engine_Define.h"
#include "CCameraActor.h"

#include "CTransform.h"
#include "CCamera.h"
#include "CInputMgr.h"
#include "GUISystem.h"
#include "CSceneMgr.h"

CCameraActor::CCameraActor()
	:m_pCamera(nullptr),m_pTransform(nullptr),m_eMode(Camera_Mode::Picking)
{
}

CCameraActor::~CCameraActor()
{
}

CCameraActor* CCameraActor::Create()
{
	CCameraActor* instance = new CCameraActor;

	if (FAILED(instance->Ready_GameObject())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CCameraActor::Ready_GameObject()
{
	m_pTransform = Add_Component<CTransform>();
	//m_pTransform->Set_Pos({ 0,0, -5.f });
	m_pCamera = Add_Component<CCamera>();

	return S_OK;
}

void CCameraActor::Update_GameObject(_float& dt)
{
	GUISystem::GetInstance()->RegisterPanel("CameraMode", [this]() {CamModePanel();});

	Update_Component(dt);
	Mode_Check(dt);
	Key_Check(dt);
}

void CCameraActor::LateUpdate_GameObject(_float& dt)
{
	LateUpdate_Component(dt);
}

void CCameraActor::Mode_Check(_float& dt)
{
	if (CInputMgr::GetInstance()->Key_Down(DIK_W)) {
		if (m_eMode == Camera_Mode::WorldMove)
			m_eMode = Camera_Mode::Picking;
		else
			m_eMode = Camera_Mode::WorldMove;
	}

	if (CInputMgr::GetInstance()->Key_Down(DIK_R)) {
		if (m_eMode == Camera_Mode::Rotate)
			m_eMode = Camera_Mode::Picking;
		else
			m_eMode = Camera_Mode::Rotate;
	}
}

void CCameraActor::Key_Check(_float& dt)
{
	if (GUISystem::GetInstance()->UsingUI()) return;

	_float Move_speed = 40;
	_float Rotate_speed = 120;

	_long x = CInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X);
	_long y = CInputMgr::GetInstance()->Get_DIMouseMove(DIMD_Y);
	_long wheel = CInputMgr::GetInstance()->Get_DIMouseMove(DIMD_Z);
	
	_vec3 Look = m_pCamera->Get_Dir();
	_vec3 up = m_pCamera->Get_Up();
	_vec3 Right = m_pCamera->Get_Right();

	_vec3 MoveForward = Look * Move_speed * wheel * dt;
	m_pTransform->Add_Pos(MoveForward);

	if (CInputMgr::GetInstance()->Mouse_Hold(DIM_LB)) {

	}
	else {
		return;
	}

	if (m_eMode == Camera_Mode::Rotate) {
		m_pCamera->Add_Yaw(x * dt * Rotate_speed);
		m_pCamera->Add_Pitch(y * dt * Rotate_speed);
	}
	else if(m_eMode == Camera_Mode::WorldMove) {
		//x만큼 오른쪽으로
		m_pTransform->Add_Pos(Right*x*Move_speed * -dt);
		//y만큼 위로
		m_pTransform->Add_Pos(up *y * Move_speed * -dt);
		//캔버스 잡아당기는 느낌 위해서 음수 곱하기
	}
}

void CCameraActor::CamModePanel()
{
	ImGui::SetNextWindowPos(ImVec2(10, 80), ImGuiCond_Once);
	ImGui::Begin("CamMode", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	
	if (ImGui::Button("Move",ImVec2(70,30)))
	{
		m_eMode = Camera_Mode::WorldMove;
	}
	ImGui::SameLine();

	if (ImGui::Button("Rotate", ImVec2(70, 30)))
	{
		m_eMode = Camera_Mode::Rotate;
	}
	ImGui::SameLine();

	if (ImGui::Button("Picking", ImVec2(70, 30)))
	{
		m_eMode = Camera_Mode::Picking;
	}
	ImGui::End();
}

void CCameraActor::Free()
{
	Release_Component();
}
