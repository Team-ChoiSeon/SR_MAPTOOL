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

<<<<<<< Updated upstream
	//instance->Set_Model(meshType);
=======
>>>>>>> Stashed changes
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

RENDER_PASS CModel::Get_RenderPass()
{
<<<<<<< Updated upstream
	//string meshKey = meshType + ".obj";
	//string matKey = meshType + ".mtl";
	//m_pMesh = CResourceMgr::GetInstance()->GetMesh(meshKey);
	//if (m_pMesh)
	//	m_pMesh->AddRef();
	//m_pMaterial = CResourceMgr::GetInstance()->GetMaterial(matKey);
	//if (m_pMaterial)
	//	m_pMaterial->AddRef();
	return S_OK;
=======
	return RENDER_PASS();
>>>>>>> Stashed changes
}
HRESULT CModel::Set_Model(const string& meshType, const string& matType)
{
<<<<<<< Updated upstream
	//  이전 리소스 해제
	if (m_pMesh || m_pMaterial) return S_OK;
	//Safe_Release(m_pMaterial);

	// 새 리소스 할당 및 AddRef
	m_pMesh = CResourceMgr::GetInstance()->GetMesh(meshType);
	if (m_pMesh)
		m_pMesh->AddRef();

	m_pMaterial = CResourceMgr::GetInstance()->GetMaterial(matType);
	if (m_pMaterial)
		m_pMaterial->AddRef();

=======
	if (!meshType.empty())
		Safe_Change(m_pMesh, CResourceMgr::GetInstance()->GetMesh(meshType));

	if (!matType.empty())
		Safe_Change(m_pMaterial, CResourceMgr::GetInstance()->GetMaterial(matType));

>>>>>>> Stashed changes
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

void CModel::Render_Panel(ImVec2 size)
{
	if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen)) // 제목
	{
		ImGui::BeginChild("## ModelChild", size, true); // 사각형 박스
		ImGui::Checkbox("Active##Model", &m_bActive);


		ImGui::EndChild();
	}
}

void CModel::Serialize(json& outJson) const
{
	if (m_pMesh)
		outJson["meshKey"] = m_pMesh->Get_Key();

	if (m_pMaterial)
		outJson["materialKey"] = m_pMaterial->Get_Key();
}

void CModel::Deserialize(const json& inJson)
{
	string meshKey, matKey;

	if (inJson.contains("meshKey"))
		meshKey = inJson["meshKey"];

	if (inJson.contains("materialKey"))
		matKey = inJson["materialKey"];

	Set_Model(meshKey, matKey);
}
