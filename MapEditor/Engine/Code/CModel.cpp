#include "Engine_Define.h"
#include "CModel.h"
#include "CRenderMgr.h"
#include "CResourceMgr.h"
#include "CMesh.h"
#include "CMaterial.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CGameObject.h"
#include "CShaderMgr.h"
#include "CCameraMgr.h"
#include "CCamera.h"

CModel::CModel()
	:m_pMesh(nullptr), m_pMaterial(nullptr), m_iShaderIndex(0), uvScale{1.f,1.f,0.f,0.f}, e_uvMode(sync)
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
	if (shader)
	{
		const D3DXVECTOR3& scale = pTransform->Get_Scale();

		D3DXMATRIX world = pTransform->Get_WorldMatrix();
		D3DXMATRIX view = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ViewMatrix();
		D3DXMATRIX proj = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ProjMatrix();
		D3DXMATRIX wvp = world * view * proj;
		shader->SetMatrix("g_matWorldViewProj", &wvp);
		if (e_uvMode == sync) {
			D3DXVECTOR4 transScale(scale.x, scale.y, 0.f, 0.f); // Z, W는 임시값
			shader->SetVector("g_UVScale", &transScale);
		}
		else {
			shader->SetVector("g_UVScale", &uvScale);
		}
	}

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
	ImGui::PushID(this); // 유일한 ID 부여

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
		ImGui::Text("UV Scale:");
		int selectedMode = static_cast<int>(e_uvMode);
		ImGui::RadioButton("SyncScale", &selectedMode, (int)sync);ImGui::SameLine();
		ImGui::RadioButton("Custom", &selectedMode, (int)custom);

		e_uvMode = static_cast<uvMode>(selectedMode);

		if (e_uvMode == 1) {
			ImGui::SliderFloat("X : ", &uvScale.x, 1.0f, 10.f, "%.3f");
			ImGui::SliderFloat("Y : ", &uvScale.y, 1.0f, 10.f, "%.3f");
		}

		ImGui::Separator();

		// 머티리얼 선택
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

		// 메쉬 선택
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

		const auto& shaderList = CShaderMgr::GetInstance()->Get_ShaderName();
		if (!shaderList.empty()) {
			if (m_iShaderIndex >= shaderList.size())
				m_iShaderIndex = 0;

			const string& selectedShader = shaderList[m_iShaderIndex];
			const string& currentShader = m_pMaterial->Get_ShaderKey(); // <- 현재 적용된 셰이더 이름

			bool notApplied = (selectedShader != currentShader);

			if (ImGui::BeginCombo("Shader", selectedShader.c_str())) {
				for (int i = 0; i < shaderList.size(); ++i) {
					bool isSelected = (m_iShaderIndex == i);
					if (ImGui::Selectable(shaderList[i].c_str(), isSelected)) {
						m_iShaderIndex = i;
						// 셰이더 실제 적용은 아래 버튼에서 할 수 있게 하거나 자동화
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (notApplied) {
				ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Not Applide"); // 주황 경고
				if (ImGui::Button("Apply")) {
					m_pMaterial->Set_Shader(selectedShader);
				}
			}
			else {
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Applied"); // 녹색 체크
			}
		}

	}

	ImGui::PopID(); // PushID에 대한 대응
}


void CModel::Serialize(json& outJson) const
{
	if (m_pMesh)
		outJson["mesh"] = m_pMesh->Get_Key();

	if (m_pMaterial)
	{
		outJson["matKey"] = m_pMaterial->Get_MaterialKey();

		const string& shaderPath = m_pMaterial->Get_ShaderKey(); 
		if (!shaderPath.empty())
			outJson["shader"] = shaderPath;
	}
}


void CModel::Deserialize(const json& inJson)
{
	string meshKey, matKey;
	string shaderPath;

	if (inJson.contains("mesh"))
		meshKey = inJson["mesh"];

	if (inJson.contains("matKey"))
		matKey = inJson["matKey"];

	if (inJson.contains("shader"))
		shaderPath = inJson["shader"];

	Set_Model(meshKey, matKey);

	if (!shaderPath.empty() && m_pMaterial)
		m_pMaterial->Set_Shader(shaderPath);
}
