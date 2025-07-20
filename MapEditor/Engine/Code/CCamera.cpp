#include "Engine_Define.h"
#include "CCamera.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CCameraMgr.h"

CCamera::CCamera()
	:m_pTransform(nullptr),m_fPitch(0),m_fYaw(0),m_fRoll(0),m_fFOV(20.0f)
{
}

CCamera::~CCamera()
{
}

CCamera* CCamera::Create()
{
	CCamera* instance = new CCamera;
	
	if (FAILED(instance->Ready_Component())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CCamera::Ready_Component()
{
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matView);
	m_vEye = { 0.f,0.f,0.f };
	m_vUp = { 0.f,1.f,0.f };
	m_vLookDir = { 0.f,0.f,1.f};

	m_fFOV = 45.f ;
	m_fAspect = (float)WINCX / WINCY;
	m_fNear = 0.1f;
	m_fFar = 200.f;

	return S_OK;
}


void CCamera::Update_Component(_float& dt)
{
	if (!m_pTransform) {
		m_pTransform = m_pOwner->Get_Component<CTransform>();
		m_pTransform->AddRef();
	}

	m_vEye = m_pTransform->Get_Pos();
	m_vLookDir = m_pTransform->Get_Look();
	m_vUp = m_pTransform->Get_Up();
	m_vRight = m_pTransform->Get_Right();

	//UpdateDirFromAngles();

	_vec3 vAt = m_vEye + m_vLookDir; //바라보는 곳은 항상 위치 + 방향

	// 뷰 행렬 생성
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &vAt, &m_vUp);
	//투영 행렬 생성
	D3DXMatrixPerspectiveFovLH(&m_matProj, D3DXToRadian(m_fFOV), m_fAspect, m_fNear, m_fFar);
}

void CCamera::LateUpdate_Component(_float& dt)
{
}

CComponent* CCamera::Clone() const
{
	return nullptr;
}

void CCamera::Set_View(_vec3 _vEye, _vec3 _vUP, _vec3 _vDir)
{
	m_vEye = _vEye;
	m_vUp = _vUP;
	m_vLookDir = _vDir;
}

void CCamera::Set_Proj(_float _FOV, _float _near, _float _far)
{
	m_fFOV = _FOV;
	m_fNear = _near;
	m_fFar = _far;
}

void CCamera::Set_YawPitchRoll(_vec3 axis)
{
	//m_fYaw = axis.x; // Y축 회전
	//m_fPitch = axis.y; // X축 회전
	//m_fRoll = axis.z; // Z축 회전
}

void CCamera::Add_Yaw(_float angle)
{
	m_fYaw += angle;
}

void CCamera::Add_Pitch(_float angle)
{
	m_fPitch += angle;
	m_fPitch = max(-89.f, min(89.f, m_fPitch));
}

//구면 좌표를 계산 
void CCamera::UpdateDirFromAngles()
{ //짐벌락(Gimbal Lock) 회피에 유리한 구조
	//_vec3 camDir;
	////구면 좌표계 공식
	//camDir.x = cosf(D3DXToRadian(m_fPitch)) * cosf(D3DXToRadian(m_fYaw));
	//camDir.y = sinf(D3DXToRadian(m_fPitch));
	//camDir.z = cosf(D3DXToRadian(m_fPitch)) * sinf(D3DXToRadian(m_fYaw));
	//
	//D3DXVec3Normalize(&camDir, &camDir); //방향 벡터로.
	//m_vLookDir = camDir;

	// Right = Up × Look
	_vec3 vRight;
	_vec3 worldUp = { 0.f, 1.f, 0.f };
	D3DXVec3Cross(&vRight, &worldUp, &m_vLookDir);//월드업 X 바라보는 방향- > 우측 벡터
	D3DXVec3Normalize(&vRight, &vRight);

	// Up = Look × Right
	D3DXVec3Cross(&m_vUp, &m_vLookDir, &vRight);;// 우측 벡터와 바라보는 방향-> 업벡터
	D3DXVec3Normalize(&m_vUp, &m_vUp);
}

void CCamera::Free()
{
	Safe_Release(m_pTransform);
}

string CCamera::Get_ComponentName() const
{
	return "CCamera";
}

void CCamera::Render_Panel(ImVec2 size)
{
	ImGui::SetNextItemOpen(false, ImGuiCond_Once);

	ImGui::Checkbox("##ActiveCamera", &m_bActive); ImGui::SameLine();

	if (ImGui::CollapsingHeader("Camera"))
	{
		Delete_ComponentPane();

		bool isMain = (CCameraMgr::GetInstance()->Get_MainCamera() == this);
		bool clicked = ImGui::Selectable("Set Camera##CameraToggle", isMain, ImGuiSelectableFlags_DontClosePopups);

		if (clicked) {
			if (isMain)
				CCameraMgr::GetInstance()->Set_MainCamera(nullptr); // 해제
			else
				CCameraMgr::GetInstance()->Set_MainCamera(this);     // 설정
		}

		ImGui::Separator();

		//mGui::Text("View Angles");
		//mGui::SliderFloat("Yaw", &m_fYaw, -180.0f, 180.0f);
		//mGui::SliderFloat("Pitch", &m_fPitch, -89.0f, 89.0f);
		//mGui::SliderFloat("Roll", &m_fRoll, -180.0f, 180.0f);

		ImGui::Separator();
		ImGui::Text("Projection");
		ImGui::SliderFloat("FOV", &m_fFOV, 20.0f, 120.0f);
		ImGui::SliderFloat("Near", &m_fNear, 0.01f, 10.0f);
		ImGui::SliderFloat("Far", &m_fFar, 10.0f, 1000.0f);

	}
}

void CCamera::Serialize(json& outJson) const
{
	outJson["yaw"] = m_fYaw;
	outJson["pitch"] = m_fPitch;
	outJson["roll"] = m_fRoll;

	outJson["fov"] = m_fFOV;
	outJson["near"] = m_fNear;
	outJson["far"] = m_fFar;

	outJson["eye"] = { m_vEye.x, m_vEye.y, m_vEye.z };
	outJson["look"] = { m_vLookDir.x, m_vLookDir.y, m_vLookDir.z };
	outJson["up"] = { m_vUp.x, m_vUp.y, m_vUp.z };
}

void CCamera::Deserialize(const json& inJson)
{
	if (inJson.contains("yaw"))   m_fYaw = inJson["yaw"];
	if (inJson.contains("pitch")) m_fPitch = inJson["pitch"];
	if (inJson.contains("roll"))  m_fRoll = inJson["roll"];

	if (inJson.contains("fov"))   m_fFOV = inJson["fov"];
	if (inJson.contains("near"))  m_fNear = inJson["near"];
	if (inJson.contains("far"))   m_fFar = inJson["far"];

	if (inJson.contains("eye")) {
		const auto& e = inJson["eye"];
		m_vEye = { e[0], e[1], e[2] };
	}

	if (inJson.contains("look")) {
		const auto& l = inJson["look"];
		m_vLookDir = { l[0], l[1], l[2] };
	}

	if (inJson.contains("up")) {
		const auto& u = inJson["up"];
		m_vUp = { u[0], u[1], u[2] };
	}
}
