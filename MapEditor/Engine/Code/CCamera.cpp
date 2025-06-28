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

	_vec3 vAt = m_vEye + m_vLookDir; //�ٶ󺸴� ���� �׻� ��ġ + ����

	// �� ��� ����
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &vAt, &m_vUp);
	//���� ��� ����
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

//���� ��ǥ�� ��� 
void CCamera::UpdateDirFromAngles()
{ //������(Gimbal Lock) ȸ�ǿ� ������ ����
	_vec3 camDir;
	//���� ��ǥ�� ����
	camDir.x = cosf(D3DXToRadian(m_fPitch)) * cosf(D3DXToRadian(m_fYaw));
	camDir.y = sinf(D3DXToRadian(m_fPitch));
	camDir.z = cosf(D3DXToRadian(m_fPitch)) * sinf(D3DXToRadian(m_fYaw));
	D3DXVec3Normalize(&camDir, &camDir); //���� ���ͷ�.
	m_vLookDir = camDir;

	// Right = Up �� Look
	_vec3 vRight;
	_vec3 worldUp = { 0.f, 1.f, 0.f };
	D3DXVec3Cross(&vRight, &worldUp, &m_vLookDir);//����� X �ٶ󺸴� ����- > ���� ����
	D3DXVec3Normalize(&vRight, &vRight);

	// Up = Look �� Right
	D3DXVec3Cross(&m_vUp, &m_vLookDir, &vRight);;// ���� ���Ϳ� �ٶ󺸴� ����-> ������
	D3DXVec3Normalize(&m_vUp, &m_vUp);
}

void CCamera::Free()
{
	Safe_Release(m_pTransform);
}

