#include "Engine_Define.h"
#include "CLight.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CLightMgr.h"


CLight::CLight()
	:m_iOption(0)
{
}

CLight::~CLight()
{
}

CLight* CLight::Create()
{
	CLight* instance = new CLight;

	if (FAILED(instance->Ready_Component())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	CLightMgr::GetInstance()->Add_Light(instance);
	return instance;
}

HRESULT CLight::Ready_Component()
{
	ZeroMemory(&m_tLight, sizeof(m_tLight)); 
	return S_OK;
}

void CLight::Update_Component(_float& dt)
{
	if (!m_pOwner) return;

	m_tLight.Position = m_pOwner->Get_Component<CTransform>()->Get_Pos();

	switch (m_iOption)
	{
	case 0:
		m_tLight.Type = D3DLIGHT_POINT;
		break;
	case 1:
		m_tLight.Type = D3DLIGHT_DIRECTIONAL;
		break;
	case 2:
		m_tLight.Type = D3DLIGHT_SPOT;
		break;
	default:
		m_tLight.Type = D3DLIGHT_POINT;
		break;
	}

}

void CLight::LateUpdate_Component(_float& dt)
{

}

void CLight::Render_Panel(ImVec2 size)
{
	ImGui::SetNextItemOpen(false, ImGuiCond_Once);

	ImGui::Checkbox("##ActiveLight", &m_bActive); ImGui::SameLine();

	if (ImGui::CollapsingHeader("Light"))
	{
		Delete_ComponentPane();

		m_iOption = 0; // 현재 선택된 인덱스 (0, 1, 2 등)
		ImGui::RadioButton("POINT", &m_iOption, 0);
		ImGui::RadioButton("DIRECTIONAL", &m_iOption, 1);
		ImGui::RadioButton("SPOT", &m_iOption, 2);
		ImGui::Separator();
		ImGui::ColorEdit3("Diffuse Color", (float*)&m_tLight.Diffuse); // RGB 슬라이더
		ImGui::ColorEdit3("Specular Color", (float*)&m_tLight.Specular); // RGB 슬라이더
		ImGui::ColorEdit3("Ambient Color", (float*)&m_tLight.Ambient); // RGB 슬라이더
		ImGui::Separator();

		switch (m_iOption)
		{
		case 0:
			Render_Point();
			break;
		case 1:
			Render_Directional();
			break;
		case 2:
			Render_Spot();
			break;
		default:
			Render_Point();
			break;
		}
	}
}

CComponent* CLight::Clone() const
{
	return nullptr;
}

void CLight::Serialize(json& outJson) const
{
}

void CLight::Deserialize(const json& inJson)
{
}

void CLight::Render_Directional()
{
		ImGui::DragFloat3("Direction", (float*)&m_tLight.Direction, 0.1f);
}

void CLight::Render_Point()
{
	ImGui::DragFloat("Range", (float*)&m_tLight.Range, 1.0f);
	ImGui::DragFloat("Falloff", (float*)&m_tLight.Falloff, 1.0f);
	ImGui::Separator();
	ImGui::DragFloat("Attenuation0", (float*)&m_tLight.Attenuation0, 0.01f);
	ImGui::DragFloat("Attenuation1", (float*)&m_tLight.Attenuation1, 0.01f);
	ImGui::DragFloat("Attenuation2", (float*)&m_tLight.Attenuation2, 0.01f);
}

void CLight::Render_Spot()
{
	ImGui::DragFloat3("Direction", (float*)&m_tLight.Direction, 0.1f);
	ImGui::DragFloat("Range", (float*)&m_tLight.Range, 1.0f);
	ImGui::DragFloat("Falloff", (float*)&m_tLight.Falloff, 1.0f);
	ImGui::Separator();
	ImGui::DragFloat("Attenuation0", (float*)&m_tLight.Attenuation0, 0.01f);
	ImGui::DragFloat("Attenuation1", (float*)&m_tLight.Attenuation1, 0.01f);
	ImGui::DragFloat("Attenuation2", (float*)&m_tLight.Attenuation2, 0.01f);
	ImGui::DragFloat("Theta", (float*)&m_tLight.Theta, 0.01f);
	ImGui::DragFloat("Phi", (float*)&m_tLight.Phi, 0.01f);
}

void CLight::Free()
{
	CLightMgr::GetInstance()->Remove_Light(this);
}