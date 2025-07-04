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
	return RENDER_PASS();
}
HRESULT CModel::Set_Model(const string& meshType, const string& matType)
{

	if (!meshType.empty())
		Safe_Change(m_pMesh, CResourceMgr::GetInstance()->GetMesh(meshType));

	if (!matType.empty())
		Safe_Change(m_pMaterial, CResourceMgr::GetInstance()->GetMaterial(matType));

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
