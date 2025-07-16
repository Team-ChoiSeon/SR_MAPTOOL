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
#include "CInputMgr.h"
#include "CCamera.h"
#include "CLightMgr.h"

CModel::CModel()
	:m_pMesh(nullptr), m_pMaterial(nullptr), m_iShaderIndex(0), m_uvScale{1.f,1.f,0.f,0.f}, e_uvMode(sync)
	, uvPos{0,0,0,0}, m_fspeed(0.f)
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
	uvPos.x += dt* m_fspeed;
	uvPos.y += dt * m_fspeed;
}

void CModel::LateUpdate_Component(_float& dt)
{
	CRenderMgr::GetInstance()->Add_ModelRenderer(this);
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

		//D3DXMATRIX wvp = world * view * proj;
		//shader->SetMatrix("g_matWorldViewProj", &wvp);

		shader->SetMatrix("g_matWorld", &world);
		shader->SetMatrix("g_matView", &view);
		shader->SetMatrix("g_matProj", &proj);
		
		D3DLIGHT9 mainLight = CLightMgr::GetInstance()->Get_MainLight();
		// 라이트 방향은 -Direction (픽셀에서 빛이 향하는 것, 입사광)
		D3DXVECTOR3 lightDir = mainLight.Direction;
		lightDir *= -1;
		D3DXVec3Normalize(&lightDir, &lightDir);

		shader->SetVector("g_LightDir", reinterpret_cast<D3DXVECTOR4*>(&lightDir)); 
		shader->SetVector("g_LightColor", reinterpret_cast<D3DXVECTOR4*>(&mainLight.Diffuse));
		shader->SetVector("g_Ambient", reinterpret_cast<D3DXVECTOR4*>(&mainLight.Ambient));

		_vec4 tmp = { 1.f,1.f,0.f,0.f };
		if (m_uvScale != tmp)
			e_uvMode = custom;

		if (e_uvMode == sync ) {
			D3DXVECTOR4 transScale(scale.x, scale.x, scale.x, 0.f); // Z, W는 임시값
			shader->SetVector("g_UVScale", &transScale);
		}
		else {
			shader->SetVector("g_UVScale", &m_uvScale);
		}
		D3DLIGHT9 tLight=	CLightMgr::GetInstance()->Get_MainLight();
		D3DXVECTOR4 LightDir(tLight.Direction.x, tLight.Direction.y, tLight.Direction.z, 0.f); // W는 임시값
		shader->SetVector("g_LightDir", &LightDir);
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
		if (m_pMaterial) {
			if (CTexture* diffuse = m_pMaterial->Get_Diffuse()) {
				LPDIRECT3DTEXTURE9 pTex = diffuse->Get_Handle();
				if (pTex) {
					ImGui::Text("Diffuse Texture:");
					ImGui::Image((ImTextureID)pTex, ImVec2(64, 64));
				}
			}
			if (CTexture* normal = m_pMaterial->Get_Normal()) {
				LPDIRECT3DTEXTURE9 pTex = normal->Get_Handle();
				if (pTex) {
					ImGui::Text("Normal Texture:");
					ImGui::Image((ImTextureID)pTex, ImVec2(64, 64));
				}
			}
		}
		else {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Matrial Handle is null");
		}

		ImGui::SliderFloat("uvSpeed : ", &m_fspeed, 0.001f, 1.f, "%.3f");

		ImGui::Separator();
		ImGui::Text("UV Scale:");
		int selectedMode = static_cast<int>(e_uvMode);
		ImGui::RadioButton("SyncScale", &selectedMode, (int)sync);ImGui::SameLine();
		ImGui::RadioButton("Custom", &selectedMode, (int)custom);

		e_uvMode = static_cast<uvMode>(selectedMode);

		if (e_uvMode == 1) {
			ImGui::Text("UV Scale");

			ImGui::Text("X"); ImGui::SameLine();
			ImGui::SetNextItemWidth(150);
			ImGui::DragFloat("##UVX", &m_uvScale.x, 0.1f, 1.0f, 30.f, "%.3f");
			ImGui::Text("Y"); ImGui::SameLine();
			ImGui::SetNextItemWidth(150);
			ImGui::DragFloat("##UVY", &m_uvScale.y, 0.1f, 1.0f, 30.f, "%.3f");
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
		if (!shaderPath.empty()) {
			outJson["shader"] = shaderPath;
			outJson["uvScale"] = { m_uvScale.x, m_uvScale.y, m_uvScale.z, m_uvScale.w };
		}
	}
}

void CModel::Deserialize(const json& inJson)
{
	string meshKey = "DirtObj.obj";
	string matKey = "DirtObj.mtl";
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

	// uvScale 역직렬화
	if (inJson.contains("uvScale") && inJson["uvScale"].is_array()) {
		const auto& uvArr = inJson["uvScale"];
		if (uvArr.size() > 0) m_uvScale.x = uvArr[0].get<float>();
		if (uvArr.size() > 1) m_uvScale.y = uvArr[1].get<float>();
		if (uvArr.size() > 2) m_uvScale.z = uvArr[2].get<float>();
		if (uvArr.size() > 3) m_uvScale.w = uvArr[3].get<float>();
	}

}
