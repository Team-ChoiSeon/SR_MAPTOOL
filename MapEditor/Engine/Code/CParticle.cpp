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

	switch (m_iMovementType)
	{
	case 0: m_MoveType = PARTICLE_MOVE_TYPE::FIRE;break;
	case 1: m_MoveType = PARTICLE_MOVE_TYPE::DUST;break;
	}

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
		// 알파 감소
		float lifeRatio = 1.f - (particle.fAge / particle.fLifeTime); // 1 → 0
		lifeRatio = max(0.f, min(1.f, lifeRatio)); // 안전한 클램프

		BYTE r = (BYTE)((particle.color >> 16) & 0xFF);
		BYTE g = (BYTE)((particle.color >> 8) & 0xFF);
		BYTE b = (BYTE)((particle.color) & 0xFF);
		BYTE a = (BYTE)(255.f * lifeRatio); // 점점 줄어듦

		particle.color = D3DCOLOR_ARGB(a, r, g, b);

		switch (particle.moveType)
		{
		case PARTICLE_MOVE_TYPE::FIRE:
			particle.vVelocity.y += 0.1f * dt; // 위로 가속
			particle.fSize += 0.3f * dt; // 점점 커짐
			break;
		case PARTICLE_MOVE_TYPE::DUST:
			particle.vVelocity.y -= 0.1f * dt; // 중력
			particle.fSize -= 0.1f * dt;
			break;
		default:
			break;
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
			particle.moveType = m_MoveType;
			particle.color = m_BaseColor; // 주황 계열
		
			switch (particle.moveType)
			{
			case PARTICLE_MOVE_TYPE::FIRE:
				particle.vVelocity = {
					randRange(-0.6f, 0.6f),
					randRange(1.0f, 2.0f),
					randRange(-0.4f, 0.4f)
				};
				break;
			case PARTICLE_MOVE_TYPE::DUST:
				particle.vVelocity = {
					randRange(-0.3f, 0.3f),
					randRange(0.1f, 0.3f),
					randRange(-0.3f, 0.3f)
				};
				break;
			default:
				break;
			}
			particle.vPos = m_vPos; //오브젝트 위치
			break; // 한 개만 emit
		}
	}
}

void CParticle::Render_Particle(LPDIRECT3DDEVICE9 pDevice)
{
	if (!pDevice || !m_pTexture || !m_pVB) return;


	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// 색상은 텍스처 RGB만
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	// 알파는 텍스처 × 정점 알파
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	// 카메라 정보
	CCamera* pCamera = CCameraMgr::GetInstance()->Get_MainCamera();
	if (!pCamera) return;

	//pDevice->SetTransform(D3DTS_VIEW, &pCamera->Get_ViewMatrix());
	//pDevice->SetTransform(D3DTS_PROJECTION, &pCamera->Get_ProjMatrix());

	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	pDevice->SetTransform(D3DTS_WORLD, &identity); // 모든 파티클에 동일 적용

	D3DXVECTOR3 camPos = pCamera->Get_Eye();

	// 텍스처 및 FVF 설정
	pDevice->SetTexture(0, m_pTexture->Get_Handle());
	pDevice->SetFVF(FVF_PARTICLE);

	// 정점 버퍼 Lock
	VTXPARTICLE* pVertices = nullptr;
	if (FAILED(m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
		return;

	int vtxCount = 0;

	for (const auto& particle : m_vecParticles)
	{
		if (!particle.bActive) continue;

		const float half = particle.fSize * 0.5f;
		const D3DXVECTOR3& c = particle.vPos;

		// 빌보드 회전 벡터 계산
		D3DXVECTOR3 vLook = camPos - c;
		D3DXVec3Normalize(&vLook, &vLook);

		D3DXVECTOR3 vUp(0.f, 1.f, 0.f);
		D3DXVECTOR3 vRight;
		D3DXVec3Cross(&vRight, &vUp, &vLook);
		D3DXVec3Normalize(&vRight, &vRight);

		// 다시 up 보정 (직교화)
		D3DXVECTOR3 vTrueUp;
		D3DXVec3Cross(&vTrueUp, &vLook, &vRight);

		// 정점 6개 (두 삼각형)
		pVertices[vtxCount++] = { c - vRight * half - vTrueUp * half,	particle.color ,{0.f, 1.f} };
		pVertices[vtxCount++] = { c - vRight * half + vTrueUp * half,	particle.color ,{0.f, 0.f} };
		pVertices[vtxCount++] = { c + vRight * half + vTrueUp * half,	particle.color ,{1.f, 0.f} };

		pVertices[vtxCount++] = { c - vRight * half - vTrueUp * half,	particle.color ,{0.f, 1.f} };
		pVertices[vtxCount++] = { c + vRight * half + vTrueUp * half,	particle.color ,{1.f, 0.f} };
		pVertices[vtxCount++] = { c + vRight * half - vTrueUp * half,	particle.color ,{1.f, 1.f} };
	}

	m_pVB->Unlock();

	// 그리기
	pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXPARTICLE));
	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, vtxCount / 3);

	// 상태 복구
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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

		ImGui::Text("Movement Type");

		ImGui::RadioButton("FIRE", &m_iMovementType, 0); ImGui::SameLine();
		ImGui::RadioButton("DUST", &m_iMovementType, 1); 
		ImGui::Separator();

		m_iPrevMaxParticles = m_iMaxParticles;
		ImGui::SliderInt("MaxParticles : ", &m_iMaxParticles, 40, 200, "%d");
		if (m_iPrevMaxParticles != m_iMaxParticles) {
			m_vecParticles.resize(m_iMaxParticles);
			m_iPrevMaxParticles = m_iMaxParticles;
		}

		ImGui::SliderFloat("SpawnInterval", &m_fSpawnInterval, 0.03f, 2.f, "%.3f");
		ImGui::SliderFloat("LifeTime", &m_fLifeTime, 0.1f, 20.f, "%.3f");
		ImGui::SliderFloat("Size", &m_fSize, 0.1f, 10.f, "%.3f");

		ImGui::Separator();
		ImGui::InputFloat3("Velocity", (_float*)&m_vVelocity, "%.2f", ImGuiInputTextFlags_ReadOnly);
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
	outJson["spawn_interval"] = m_fSpawnInterval;
	outJson["max_particles"] = m_iMaxParticles;
	outJson["life_time"] = m_fLifeTime;
	outJson["size"] = m_fSize;

	outJson["offset"] = { m_vOffset.x, m_vOffset.y, m_vOffset.z };

	outJson["base_color"] =  //a,r,g,b
	{		((BYTE)((m_BaseColor) >> 24) & 0xFF),
			((BYTE)((m_BaseColor) >> 16) & 0xFF),
			((BYTE)((m_BaseColor) >> 8) & 0xFF),
			((BYTE)((m_BaseColor) >> 0) & 0xFF)
	};

	outJson["Particle_Type"] =  m_MoveType ;

	if (!m_pTexture->GetKey().empty())
		outJson["texture_path"] = m_pTexture->GetKey();
}


void CParticle::Deserialize(const json& inJson)
{
	m_fSpawnInterval = inJson.value("spawn_interval", 1.0f);
	m_iMaxParticles = inJson.value("max_particles", 100);
	m_fLifeTime = inJson.value("life_time", 1.0f);
	m_fSize = inJson.value("size", 1.0f);
	m_MoveType = inJson.value("Particle_Type", PARTICLE_MOVE_TYPE::FIRE);

	auto off = inJson.value("offset", std::vector<float>{0.f, 0.f, 0.f});
	if (off.size() == 3) m_vOffset = { off[0], off[1], off[2] };

	auto col = inJson.value("base_color", std::vector<int>{255, 255, 255, 255});
	if (col.size() == 4)
		m_BaseColor = D3DCOLOR_ARGB(col[0], col[1], col[2], col[3]);

	if (inJson.contains("texture_path")) {
		Set_Texture(inJson["texture_path"]);
	}
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

float CParticle::randRange(float min, float max)
{
	float t = (float)rand() / (float)RAND_MAX; // 0.0f ~ 1.0f
	return min + t * (max - min);
}

