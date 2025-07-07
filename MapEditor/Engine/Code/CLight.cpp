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
		const auto& l = m_tLight;

		outJson["type"] = l.Type;
		outJson["diffuse"] = { l.Diffuse.r, l.Diffuse.g, l.Diffuse.b, l.Diffuse.a };
		outJson["ambient"] = { l.Ambient.r, l.Ambient.g, l.Ambient.b, l.Ambient.a };
		outJson["specular"] = { l.Specular.r, l.Specular.g, l.Specular.b, l.Specular.a };
		outJson["position"] = { l.Position.x, l.Position.y, l.Position.z };
		outJson["direction"] = { l.Direction.x, l.Direction.y, l.Direction.z };
		outJson["range"] = l.Range;
		outJson["attenuation"] = { l.Attenuation0, l.Attenuation1, l.Attenuation2 };
		outJson["theta"] = l.Theta;
		outJson["phi"] = l.Phi;
}

void CLight::Deserialize(const json& inJson)
{
	auto& l = m_tLight;

	l.Type = inJson.value("type", D3DLIGHT_DIRECTIONAL); // 기본값
	auto diff = inJson["diffuse"];
	l.Diffuse = { diff[0], diff[1], diff[2], diff[3] };

	auto amb = inJson["ambient"];
	l.Ambient = { amb[0], amb[1], amb[2], amb[3] };

	auto spec = inJson["specular"];
	l.Specular = { spec[0], spec[1], spec[2], spec[3] };

	auto pos = inJson["position"];
	l.Position = { pos[0], pos[1], pos[2] };

	auto dir = inJson["direction"];
	l.Direction = { dir[0], dir[1], dir[2] };

	l.Range = inJson["range"];
	auto att = inJson["attenuation"];
	l.Attenuation0 = att[0];
	l.Attenuation1 = att[1];
	l.Attenuation2 = att[2];

	l.Theta = inJson.value("theta", 0.0f);
	l.Phi = inJson.value("phi", 0.0f);
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