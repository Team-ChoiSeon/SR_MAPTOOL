#include "Engine_Define.h"
#include "CModel.h"
#include "CResourceMgr.h"
#include "CMesh.h"
#include "CMaterial.h"

CModel::CModel()
	:m_pMesh(nullptr),m_pMaterial(nullptr)
{
}

CModel::~CModel()
{
}

CModel* CModel::Create()
{
	CModel* instance = new CModel;

	if (FAILED(instance->Ready_Component())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	//instance->Set_Model(meshType);
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
	m_pMaterial->Apply(pDevice); // �ؽ�ó ���ε�
	m_pMesh->Render(pDevice);    // ����/�ε��� ������
}

CComponent* CModel::Clone() const
{
	return nullptr;
}

HRESULT CModel::Set_Model(const string& meshType)
{
	//string meshKey = meshType + ".obj";
	//string matKey = meshType + ".mtl";
	//m_pMesh = CResourceMgr::GetInstance()->GetMesh(meshKey);
	//if (m_pMesh)
	//	m_pMesh->AddRef();
	//m_pMaterial = CResourceMgr::GetInstance()->GetMaterial(matKey);
	//if (m_pMaterial)
	//	m_pMaterial->AddRef();
	return S_OK;
}
HRESULT CModel::Set_Model(const string& meshType, const string& matType)
{
	//  ���� ���ҽ� ����
	Safe_Release(m_pMesh);
	Safe_Release(m_pMaterial);

	// �� ���ҽ� �Ҵ� �� AddRef
	m_pMesh = CResourceMgr::GetInstance()->GetMesh(meshType);
	if (m_pMesh)
		m_pMesh->AddRef();

	m_pMaterial = CResourceMgr::GetInstance()->GetMaterial(matType);
	if (m_pMaterial)
		m_pMaterial->AddRef();

	return S_OK;
}

CMesh* CModel::Get_Mesh()
{
	return  m_pMesh; 
}

CMaterial* CModel::Get_Material()
{
	return m_pMaterial;
}

void CModel::Free()
{
	Safe_Release(m_pMesh);
	Safe_Release(m_pMaterial);
}
