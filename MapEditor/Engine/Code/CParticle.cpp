#include "Engine_Define.h"
#include "CParticle.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CCameraMgr.h"
#include "CCamera.h"
#include "CTexture.h"
#include "CResourceMgr.h"
#include "CRenderMgr.h"
#include "CGraphicDev.h"

CParticle::CParticle()
{
}

CParticle::~CParticle()
{
}

CParticle* CParticle::Create()
{
	CParticle* instance = new CParticle;

	if (FAILED(instance->Ready_Component())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CParticle::Ready_Component()
{
	m_vecParticles.resize(m_iMaxParticles); //파티클 생성
	LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();
	pDevice->CreateVertexBuffer(
		sizeof(VTXPARTICLE) * 6 * m_iMaxParticles,
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		FVF_PARTICLE,
		D3DPOOL_DEFAULT,
		&m_pVB,
		nullptr);
	return S_OK;
}

void CParticle::Update_Component(_float& dt)
{

	if (m_pOwner)
		m_vPos = m_pOwner->Get_Component<CTransform>()->Get_Pos() + m_vOffset;


	for (auto& particle : m_vecParticles)
	{
		if (!particle.bActive) continue;

		particle.fAge += dt; // 생존 시간 증가

		if (particle.fAge >= particle.fLifeTime)
		{
			particle.bActive = false;
			continue;
		}
		particle.vPos += particle.vVelocity * dt;
	}


}

void CParticle::LateUpdate_Component(_float& dt)
{
	m_fElapsedTime += dt;
	if (m_fElapsedTime >= m_fSpawnInterval)
	{
		Emit_Particle();
		m_fElapsedTime = 0.f;
	}

	CRenderMgr::GetInstance()->Add_ParticleRenderer(this);
}

void CParticle::Emit_Particle()
{
	for (auto& particle : m_vecParticles)
	{
		if (!particle.bActive)
		{
			particle.bActive = true;
			particle.fLifeTime = m_fLifeTime;
			particle.fSize = m_fSize;
			particle.fAge = 0;
			particle.color = m_BaseColor;
			particle.vVelocity = {
				(rand() % 100 - 50) * 0.01f,  // -0.5 ~ +0.5
				(rand() % 100) * 0.01f,       // 0 ~ +1.0 (위로 상승)
				(rand() % 100 - 50) * 0.01f   // -0.5 ~ +0.5
			};

			particle.vPos = m_vPos; //오브젝트 위치
			break; // 한 개만 emit
		}
	}
}

void CParticle::Render_Particle(LPDIRECT3DDEVICE9 pDevice)
{
	if (!pDevice || !m_pTexture) return;

	// 렌더 상태
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDevice->SetTexture(0, m_pTexture->Get_Handle());
	pDevice->SetFVF(FVF_PARTICLE);

	// 테스트 파티클: 정사각형 하나
	D3DXVECTOR3 center = { 0.f, 0.f, 0.f }; // 월드 중심에 위치
	float halfSize = 1.f;

	VTXPARTICLE quad[6] = {
		{center + D3DXVECTOR3(-halfSize, -halfSize, 0), {0.f, 1.f}, D3DCOLOR_ARGB(255, 255, 255, 255)},
		{center + D3DXVECTOR3(-halfSize,  halfSize, 0), {0.f, 0.f}, D3DCOLOR_ARGB(255, 255, 255, 255)},
		{center + D3DXVECTOR3(halfSize,  halfSize, 0), {1.f, 0.f}, D3DCOLOR_ARGB(255, 255, 255, 255)},

		{center + D3DXVECTOR3(-halfSize, -halfSize, 0), {0.f, 1.f}, D3DCOLOR_ARGB(255, 255, 255, 255)},
		{center + D3DXVECTOR3(halfSize,  halfSize, 0), {1.f, 0.f}, D3DCOLOR_ARGB(255, 255, 255, 255)},
		{center + D3DXVECTOR3(halfSize, -halfSize, 0), {1.f, 1.f}, D3DCOLOR_ARGB(255, 255, 255, 255)},
	};

	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, quad, sizeof(VTXPARTICLE));

	// 상태 복구
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetTexture(0, nullptr);

}


void CParticle::Render_Panel(ImVec2 size)
{
	ImGui::PushID(this); // 유일한 ID 부여

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	ImGui::Checkbox("##ActiveParticle", &m_bActive); ImGui::SameLine();
	if (ImGui::CollapsingHeader("Particle"))
	{
		if (m_pTexture) {
			LPDIRECT3DTEXTURE9 pTex = m_pTexture->Get_Handle();
			if (pTex) {
				ImGui::Text("Texture Preview:");
				ImGui::Image((ImTextureID)pTex, ImVec2(64, 64));
			}
		}
		else {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Texture Handle is null");
		}

		// 텍스처 선택
		const auto& textureList = CResourceMgr::GetInstance()->Get_TextureName();

		if (!textureList.empty()) {
			if (m_iTextureIndex >= textureList.size())
				m_iTextureIndex = 0;

			if (ImGui::BeginCombo("Texture", textureList[m_iTextureIndex].c_str())) {
				for (int i = 0; i < textureList.size(); ++i) {
					bool isSelected = (m_iTextureIndex == i);
					if (ImGui::Selectable(textureList[i].c_str(), isSelected)) {
						m_iTextureIndex = i;
						Set_Texture(textureList[i]);
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}

		m_iPrevMaxParticles = m_iMaxParticles;
		ImGui::SliderInt("MaxParticles : ", &m_iMaxParticles, 1, 100, "%d");
		if (m_iPrevMaxParticles != m_iMaxParticles) {
			m_vecParticles.resize(m_iMaxParticles);
			m_iPrevMaxParticles = m_iMaxParticles;
		}

		ImGui::SliderFloat("SpawnInterval", &m_fSpawnInterval, 0.1f, 1.f, "%.3f");
		ImGui::SliderFloat("LifeTime", &m_fLifeTime, 0.1f, 20.f, "%.3f");
		ImGui::SliderFloat("Size", &m_fSize, 0.1f, 10.f, "%.3f");

		ImGui::Separator();
		ImGui::Text("Direction Vectors");
		ImGui::InputFloat3("Velocity", (_float*)&m_vVelocity, "%.2f", ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Position", (_float*)&m_vPos, "%.2f", ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Offset", (_float*)&m_vOffset, "%.2f");
		// 1. float[4] 변환
		float color[4] = {
			((m_BaseColor >> 16) & 0xFF) / 255.f,
			((m_BaseColor >> 8) & 0xFF) / 255.f,
			((m_BaseColor) & 0xFF) / 255.f,
			((m_BaseColor >> 24) & 0xFF) / 255.f
		};

		if (ImGui::ColorEdit4("Base Color", color)) {
			BYTE r = static_cast<BYTE>(color[0] * 255.f);
			BYTE g = static_cast<BYTE>(color[1] * 255.f);
			BYTE b = static_cast<BYTE>(color[2] * 255.f);
			BYTE a = static_cast<BYTE>(color[3] * 255.f);
			m_BaseColor = D3DCOLOR_ARGB(a, r, g, b);
		}
	}
	ImGui::PopID(); // PushID에 대한 대응
}

CComponent* CParticle::Clone() const
{
	return nullptr;
}

void CParticle::Serialize(json& outJson) const
{

}

void CParticle::Deserialize(const json& inJson)
{

}

string CParticle::Get_ComponentName() const
{
	return "CParticle";
}

void CParticle::Set_Texture(const string& key)
{
	if (!key.empty()) {
		m_pTexture = CResourceMgr::GetInstance()->GetTexture(key);
		m_iTextureIndex = CResourceMgr::GetInstance()->Get_TextureID(key);
	}
}

void CParticle::Free()
{
	Safe_Release(m_pVB);
}
