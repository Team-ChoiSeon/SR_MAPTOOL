#include "Engine_Define.h"
#include "CModel.h"
#include "CResourceMgr.h"
#include "CMesh.h"
#include "CMaterial.h"

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

	instance->Set_Model(meshType);
	return instance;
}

HRESULT CModel::Ready_Component()
{
	return S_OK;
}

void CModel::Update_Component(_float& dt)
{
}

void CModel::LateUpdate_Component(_float& dt)
{
}

void CModel::Render(LPDIRECT3DDEVICE9 pDevice)
{
	m_pMaterial->Apply(pDevice); // 텍스처 바인드
	m_pMesh->Render(pDevice);    // 정점/인덱스 렌더링
}

CComponent* CModel::Clone() const
{
	return nullptr;
}

HRESULT CModel::Set_Model(const string& meshType)
{
	string meshKey = meshType + ".obj";
	string matKey = meshType + ".mtl";
	m_pMesh = CResourceMgr::GetInstance()->GetMesh(meshKey);
	m_pMesh->AddRef();
	m_pMaterial = CResourceMgr::GetInstance()->GetMaterial(matKey);
	m_pMaterial->AddRef();
	return S_OK;
}

CMesh* CModel::Get_Mesh()
{
	return  m_pMesh; ;
}

void CModel::Free()
{
	Safe_Release(m_pMesh);
	Safe_Release(m_pMaterial);
}
