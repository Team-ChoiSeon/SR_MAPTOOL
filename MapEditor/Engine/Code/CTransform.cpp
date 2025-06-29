#include "Engine_Define.h"
#include "CTransform.h"

CTransform::CTransform()
{
}

CTransform::~CTransform()
{
}

CTransform* CTransform::Create()
{
	CTransform* instance = new CTransform;

	if (FAILED(instance->Ready_Component())) {
		Safe_Release(instance);
		return nullptr;
	}

	return instance;
}

HRESULT CTransform::Ready_Component()
{
	D3DXMatrixIdentity(&m_WorldMat);
	m_vPos = { 0.f,0.f,0.f };
	m_vRotate = { 0.f,0.f,0.f };
	m_vScale = { 1.f,1.f,1.f };
	m_vPivot = { 0.f,0.f,0.f };
	m_vOrbit = { 0.f,0.f,0.f };
	m_vLook = { 0.f,0.f,1.f };

	//�躤�� ���ݴ� ���� �ʿ���
	m_pParent = nullptr;

	return S_OK;
}

void CTransform::Update_Component(_float& dt)
{
	D3DXMATRIX matScale, matRotateX, matRotateY, matRotateZ, matTrans;
	//ũ��
	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);
	//����
	D3DXMatrixRotationX(&matRotateX, D3DXToRadian(m_vRotate.x));
	D3DXMatrixRotationY(&matRotateY, D3DXToRadian(m_vRotate.y));
	D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(m_vRotate.z));
	//�̵�
	D3DXMatrixTranslation(&matTrans, m_vPos.x, m_vPos.y, m_vPos.z);

	m_WorldMat = matScale * matRotateX * matRotateY * matRotateZ * matTrans;
	m_WorldPosMat = matScale * matTrans;
	//-----------------------------------------------------------//
	//���� 
	D3DXMATRIX matToPivot, matFromPivot, matOrbitX, matOrbitY, matOrbitZ;
	D3DXMatrixTranslation(&matFromPivot, -m_vPivot.x, -m_vPivot.y, -m_vPivot.z);
	D3DXMatrixRotationX(&matOrbitX, D3DXToRadian(m_vOrbit.x));
	D3DXMatrixRotationY(&matOrbitY, D3DXToRadian(m_vOrbit.y));
	D3DXMatrixRotationZ(&matOrbitZ, D3DXToRadian(m_vOrbit.z));
	D3DXMatrixTranslation(&matToPivot, m_vPivot.x, m_vPivot.y, m_vPivot.z);

	m_WorldMat *= matFromPivot * matOrbitX * matOrbitY * matOrbitZ * matToPivot;

	//�θ�
	if (m_pParent) {
		m_WorldMat *= m_pParent->Get_WorldMatrix();
	}
}

void CTransform::LateUpdate_Component(_float& dt)
{


}

CComponent* CTransform::Clone() const
{
	return nullptr;
}

void CTransform::Set_Parent(CTransform* transform)
{
	m_pParent = transform;
}

void CTransform::Set_Pos(_vec3 pos)
{
	m_vPos = pos;
}

void CTransform::Set_Scale(_vec3 scale)
{
	m_vScale = scale;
}

void CTransform::Set_Rotate(_vec3 rot)
{
	m_vRotate = rot;
}

void CTransform::Set_Pivot(_vec3 pivot)
{
	m_vPivot = pivot;
}

void CTransform::Set_Orbit(_vec3 orbit)
{
	m_vOrbit = orbit;
}

void CTransform::Set_Look(_vec3 look)
{
	m_vLook = look;
}

void CTransform::Add_Pos(_vec3 pos)
{
	m_vPos += pos;
}

void CTransform::Add_Scale(_vec3 scale)
{
	m_vScale += scale;
}

void CTransform::Add_Rotate(_vec3 rot)
{
	m_vRotate += rot;
}

void CTransform::Add_Pivot(_vec3 pivot)
{
	m_vPivot += pivot;
}

void CTransform::Add_Orbit(_vec3 orbit)
{
	m_vOrbit += orbit;
}

void CTransform::Free()
{
}