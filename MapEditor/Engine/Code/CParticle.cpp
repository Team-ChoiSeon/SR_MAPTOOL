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
				(rand() % 100 - 50) * 0.01f, // X축: -0.5 ~ 0.5
				(rand() % 100) * 0.01f,      // Y축: 0 ~ 1.0
				(rand() % 100 - 50) * 0.01f  // Z축: -0.5 ~ 0.5
			};

			particle.vPos = m_vPos; //오브젝트 위치
			break; // 한 개만 emit
		}
	}
}

void CParticle::Render_Particle(LPDIRECT3DDEVICE9 pDevice)
{
	if (!pDevice || !m_pTexture || !m_pVB) return;

	//pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	CCamera* camera = CCameraMgr::GetInstance()->Get_MainCamera();
	pDevice->SetTransform(D3DTS_VIEW, &camera->Get_ViewMatrix());
	pDevice->SetTransform(D3DTS_PROJECTION, &camera->Get_ProjMatrix());
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	D3DXVECTOR3 camPos = camera->Get_Eye();
	D3DXVECTOR3 camLook = camera->Get_Dir();
	//카메라가 보는 방향. 그 방향에서 헋...

	pDevice->SetTexture(0, m_pTexture->Get_Handle());
	pDevice->SetFVF(FVF_PARTICLE);

	VTXPARTICLE* pVertices = nullptr;
	if (FAILED(m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
		return;

	int vtxCount = 0;

	CCamera* pCamera = CCameraMgr::GetInstance()->Get_MainCamera();
	// 1. 뷰 행렬의 역행렬에서 회전만 뽑아서
	_matrix invView;
	D3DXMatrixInverse(&invView, nullptr, &camera->Get_ViewMatrix());

	_vec3 vRight = { invView._11, invView._12, invView._13 };
	_vec3 vUp = { invView._21, invView._22, invView._23 };
	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);



	// 2. 파티클 정점 구성 (빌보드 평면)
	for (const auto& particle : m_vecParticles)
	{
		if (!particle.bActive) continue;

		const float half = particle.fSize * 0.5f;
		const D3DXVECTOR3& c = particle.vPos;

		pVertices[vtxCount++] = { c - vRight * half - vUp * half, {0.f, 1.f}, particle.color };
		pVertices[vtxCount++] = { c - vRight * half + vUp * half, {0.f, 0.f}, particle.color };
		pVertices[vtxCount++] = { c + vRight * half + vUp * half, {1.f, 0.f}, particle.color };

		pVertices[vtxCount++] = { c - vRight * half - vUp * half, {0.f, 1.f}, particle.color };
		pVertices[vtxCount++] = { c + vRight * half + vUp * half, {1.f, 0.f}, particle.color };
		pVertices[vtxCount++] = { c + vRight * half - vUp * half, {1.f, 1.f}, particle.color };
	}

	m_pVB->Unlock();

	//  그리기
	pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXPARTICLE));
	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, vtxCount / 3);

	// 상태 복구
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
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
		CCamera* pCamera = CCameraMgr::GetInstance()->Get_MainCamera();
		D3DXVECTOR3 camRight = pCamera->Get_Right();
		D3DXVECTOR3 camUp = pCamera->Get_Up();
		ImGui::InputFloat3("camUp", (_float*)&camUp, "%.2f", ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("camRight", (_float*)&camRight, "%.2f", ImGuiInputTextFlags_ReadOnly);

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
