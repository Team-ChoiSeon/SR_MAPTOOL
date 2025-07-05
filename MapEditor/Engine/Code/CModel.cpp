#include "Engine_Define.h"
#include "CModel.h"
#include "CRenderMgr.h"
#include "CResourceMgr.h"
#include "CMesh.h"
#include "CMaterial.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CGameObject.h"

CModel::CModel()
	:m_pMesh(nullptr), m_pMaterial(nullptr)
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
	CRenderMgr::GetInstance()->Add_Renderer(this);
}

void CModel::Render(LPDIRECT3DDEVICE9 pDevice)
{

	if (!pDevice || !m_pOwner || !m_pMesh || !m_pMaterial)
		return;

	CTransform* pTransform = m_pOwner->Get_Component<CTransform>();
	if (!pTransform)
		return;
	pDevice->SetTransform(D3DTS_WORLD, &pTransform->Get_WorldMatrix());

	LPD3DXEFFECT shader = m_pMaterial->Get_Effect();
	UINT passCount = 0;
	m_pMaterial->Apply(pDevice);

	if (shader) {
		shader->Begin(&passCount, 0);
		for (UINT i = 0; i < passCount; ++i) {
			shader->BeginPass(i);
			m_pMesh->Render(pDevice);
			shader->EndPass();
		}
		shader->End();
	}
	else {
		m_pMesh->Render(pDevice);
	}
}

CComponent* CModel::Clone() const
{
	return nullptr;
}

RENDER_PASS CModel::Get_RenderPass()
{
	return RENDER_PASS::RP_OPAQUE;
}

HRESULT CModel::Set_Model(const string& meshType, const string& matType)
{

	if (!meshType.empty()) {
		Safe_Change(m_pMesh, CResourceMgr::GetInstance()->GetMesh(meshType));
		m_iMeshIndex = CResourceMgr::GetInstance()->Get_MeshID(meshType);
	}

	if (!matType.empty()) {
		Safe_Change(m_pMaterial, CResourceMgr::GetInstance()->GetMaterial(matType));
		m_iMaterialIndex = CResourceMgr::GetInstance()->Get_MaterialID(matType);
	}

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

AABB CModel::Get_AABB()
{
	return m_pMesh->Get_AABBBOX();
}

string CModel::Get_ComponentName() const
{
	return "CModel";
}

void CModel::Free()
{
	Safe_Release(m_pMesh);
	Safe_Release(m_pMaterial);
}

void CModel::Render_Panel(ImVec2 size)
{
	ImGui::PushID(this); // ������ ID �ο�

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	ImGui::Checkbox("##ActiveModel", &m_bActive); ImGui::SameLine();
	if (ImGui::CollapsingHeader("Model"))
	{
		if (CTexture* diffuse = m_pMaterial->Get_Diffuse()) {
			LPDIRECT3DTEXTURE9 pTex = diffuse->Get_Handle();
			if (pTex) {
				ImGui::Text("Texture Preview:");
				ImGui::Image((ImTextureID)pTex, ImVec2(64, 64));
			}
		}

		ImGui::Separator();

		// ��Ƽ���� ����
		const auto& materialList = CResourceMgr::GetInstance()->Get_MaterialName();
		if (!materialList.empty()) {
			if (m_iMaterialIndex >= materialList.size())
				m_iMaterialIndex = 0;

			if (ImGui::BeginCombo("Material", materialList[m_iMaterialIndex].c_str())) {
				for (int i = 0; i < materialList.size(); ++i) {
					bool isSelected = (m_iMaterialIndex == i);
					if (ImGui::Selectable(materialList[i].c_str(), isSelected)) {
						m_iMaterialIndex = i;
						Set_Model("", materialList[i]);
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}

		ImGui::Separator();

		// �޽� ����
		const auto& meshList = CResourceMgr::GetInstance()->Get_MeshName();
		if (!meshList.empty()) {
			if (m_iMeshIndex >= meshList.size())
				m_iMeshIndex = 0;

			if (ImGui::BeginCombo("Mesh", meshList[m_iMeshIndex].c_str())) {
				for (int i = 0; i < meshList.size(); ++i) {
					bool isSelected = (m_iMeshIndex == i);
					if (ImGui::Selectable(meshList[i].c_str(), isSelected)) {
						m_iMeshIndex = i;
						Set_Model(meshList[i], "");
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
	}

	ImGui::PopID(); // PushID�� ���� ����
}


void CModel::Serialize(json& outJson) const
{
	if (m_pMesh)
		outJson["mesh"] = m_pMesh->Get_Key();

	if (m_pMaterial)
		outJson["matKey"] = m_pMaterial->Get_MaterialKey();
}

void CModel::Deserialize(const json& inJson)
{
	string meshKey, matKey;

	if (inJson.contains("mesh"))
		meshKey = inJson["mesh"];

	if (inJson.contains("matKey"))
		matKey = inJson["matKey"];

	Set_Model(meshKey, matKey);
}
