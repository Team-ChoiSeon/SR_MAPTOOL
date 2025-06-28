#include "Engine_Define.h"
#include "CCamera.h"
#include "CGameObject.h"
#include "CTransform.h"

CCamera::CCamera()
	:m_pTransform(nullptr),m_fPitch(0),m_fYaw(0),m_fRoll(0)
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

	m_fAspect = 45.f;
	m_fFOV = (float)WINCX / WINCY;
	m_fNear = 0.1f;
	m_fFar = 100.f;

	return S_OK;
}


void CCamera::Update_Component(_float& dt)
{
	if (!m_pTransform) {
		m_pTransform = m_pOwner->Get_Component<CTransform>();
		m_pTransform->AddRef();
	}

	m_vEye = m_pTransform->Get_Pos();
	UpdateDirFromAngles();

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

void CCamera::Set_View(_vec3 _vEye, _vec3 _vUP)
{
	m_vEye = _vEye;
	m_vUp = _vUP;
}

void CCamera::Set_Proj(_float _FOV, _float _aspect, _float _near, _float _far)
{
	m_fFOV = _FOV;
	m_fAspect = _aspect;
	m_fNear = _near;
	m_fFar = _far;
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
	_vec3 camDir;
	//구면 좌표계 공식
	camDir.x = cosf(D3DXToRadian(m_fPitch)) * cosf(D3DXToRadian(m_fYaw));
	camDir.y = sinf(D3DXToRadian(m_fPitch));
	camDir.z = cosf(D3DXToRadian(m_fPitch)) * sinf(D3DXToRadian(m_fYaw));
	D3DXVec3Normalize(&camDir, &camDir); //방향 벡터로.
	m_vLookDir = camDir;

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

