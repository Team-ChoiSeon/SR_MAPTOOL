#include "Engine_Define.h"
#include "CModel.h"
#include "CResourceMgr.h"
#include "IMesh.h"

CModel::CModel()
	:m_pMesh(nullptr)
{
}

CModel::~CModel()
{
}

CModel* CModel::Create(const string& meshType)
{
	CModel* instance = new CModel;

	if (FAILED(instance->Ready_Component())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	instance->Set_Mesh(meshType);
	return instance;
}

HRESULT CModel::Ready_Component()
{
	return S_OK;
}

void CModel::Update_Component(float& dt)
{
}

void CModel::LateUpdate_Component(float& dt)
{
}

void CModel::Render()
{

}

CComponent* CModel::Clone() const
{
	return nullptr;
}

HRESULT CModel::Set_Mesh(const string& meshType)
{
	m_pMesh = CResourceMgr::GetInstance()->Get_Mesh(meshType);
	m_pMesh->AddRef();

	return S_OK;
}

void CModel::Free()
{
	Safe_Release(m_pMesh);
}
