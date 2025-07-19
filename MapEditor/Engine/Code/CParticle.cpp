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

CParticle::CParticle() :m_fAngle(0)
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
	m_vecParticles.resize(m_iMaxParticles); //��ƼŬ ����
	m_pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();
	m_pDevice->CreateVertexBuffer(
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
	//������ Ÿ�� ����
	switch (m_iMovementType)
	{
	case 0: m_eMoveType = PARTICLE_MOVE_TYPE::FIRE;break;
	case 1: m_eMoveType = PARTICLE_MOVE_TYPE::DUST;break;
	case 2: m_eMoveType = PARTICLE_MOVE_TYPE::BREATH;break;
	case 3: m_eMoveType = PARTICLE_MOVE_TYPE::SPREAD;break;
	case 4: m_eMoveType = PARTICLE_MOVE_TYPE::RADIAL;break;
	}

	//�߻� ��ġ ����
	if (m_pOwner)
		m_vPos = m_pOwner->Get_Component<CTransform>()->Get_Pos() + m_vOffset;

	//�� ��ƼŬ ����
	for (auto& particle : m_vecParticles)
	{
		if (!particle.bActive) continue;

		particle.fAge += dt; // ���� �ð� ����

		if (particle.fAge >= particle.fLifeTime)
		{
			particle.bActive = false; //���� �ð� ���� �� ��Ȱ��ȭ
			continue;
		}
		// ���� ����
		float lifeRatio = 1.f - (particle.fAge / particle.fLifeTime); // 1 �� 0
		lifeRatio = max(0.f, min(1.f, lifeRatio)); // ������ Ŭ����

		BYTE r = (BYTE)((particle.color >> 16) & 0xFF);
		BYTE g = (BYTE)((particle.color >> 8) & 0xFF);
		BYTE b = (BYTE)((particle.color) & 0xFF);
		BYTE a = (BYTE)(255.f * lifeRatio); // ���� �پ��

		particle.color = D3DCOLOR_ARGB(a, r, g, b);

		switch (particle.moveType)
		{
		case PARTICLE_MOVE_TYPE::FIRE:
			particle.vVelocity.y += 0.1f * dt; // ���� ����
			particle.fSize += 0.3f * dt; // ���� Ŀ��
			break;
		case PARTICLE_MOVE_TYPE::DUST:
			particle.vVelocity.y -= 0.1f * dt; // �߷�
			particle.fSize -= 0.1f * dt;
			break;
		case PARTICLE_MOVE_TYPE::BREATH:
			particle.vVelocity *= 0.9998; //����
			particle.fSize += 0.3f * dt;
			break;
		case PARTICLE_MOVE_TYPE::SPREAD:
			particle.fSize += 0.3f * dt;
			break;
		case PARTICLE_MOVE_TYPE::RADIAL:
			particle.fSize += 0.4f * dt;
			break;
		default:
			break;
		}

		particle.vPos += particle.vVelocity * m_fSpeed*dt; //���ǵ常ŭ �̵�
	}
}

void CParticle::LateUpdate_Component(_float& dt)
{
	m_fElapsedTime += dt;

	if (!m_bLoop && m_eNowState == EMITTING)
		m_fElapsedEmit += dt;

	if (m_eNowState == PARTICLE_STATE::EMITTING && m_fElapsedTime >= m_fSpawnInterval)
	{
		Emit_Particle();
		m_fElapsedTime = 0.f;
	}

	if (!m_bLoop && m_fElapsedEmit >= m_fEmitLife) {
		m_eNowState = STOPPING;
	}

	if (m_eNowState == PARTICLE_STATE::STOPPING && Particle_Count() == 0)
		m_eNowState = PARTICLE_STATE::NONEACTIVE;

	CRenderMgr::GetInstance()->Add_ParticleRenderer(this);
}

void CParticle::Emit_Particle()
{

	if (m_eNowState == STOPPING) return;

	if (m_eMoveType == PARTICLE_MOVE_TYPE::RADIAL) {
		int count = 0;
		_matrix RotateMat;

		for (auto& particle : m_vecParticles) {
			if (!particle.bActive)
			{
				particle.bActive = true;
				particle.fLifeTime = m_fLifeTime;
				particle.fSize = m_fSize;
				particle.fAge = 0;
				particle.moveType = m_eMoveType;
				particle.color = m_BaseColor; // ��Ȳ �迭
				_vec3 defaultDir ; // ���� ����
				if (fabs(m_vAxis.y) > 0.99f)
					defaultDir = D3DXVECTOR3(1.f, 0.f, 0.f); // X��
				else
					defaultDir = D3DXVECTOR3(0.f, 1.f, 0.f); // Y��

				D3DXVec3Normalize(&m_vAxis,&m_vAxis);
				// ȸ�� ��� ����: m_vAxis ���� �������� m_fAngle��ŭ ȸ��
				D3DXMatrixRotationAxis(&RotateMat, &m_vAxis, D3DXToRadian(m_fAngle));

				_vec3 rotatedDir;
				D3DXVec3TransformNormal(&rotatedDir, &defaultDir, &RotateMat);
				particle.vVelocity = rotatedDir;

				m_fAngle = (360.f / max(m_EmitCount,1)) * count;

				if (m_fAngle >= 360.f)
					m_fAngle = 0.f;

				count++;
			}
			particle.vPos = m_vPos; //������Ʈ ��ġ

			if (count >= m_EmitCount) break;
		}
	}
	else {
		for (auto& particle : m_vecParticles)
		{
			if (!particle.bActive)
			{
				particle.bActive = true;
				particle.fLifeTime = m_fLifeTime;
				particle.fSize = m_fSize;
				particle.fAge = 0;
				particle.moveType = m_eMoveType;
				particle.color = m_BaseColor; // ��Ȳ �迭

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
				case PARTICLE_MOVE_TYPE::BREATH:
					particle.vVelocity = {
						m_vVelocity.x * randRange(1.1f, 2.6f),
						m_vVelocity.y * randRange(1.1f, 2.6f),
						m_vVelocity.z * randRange(1.1f, 2.6f)
					};
					break;

				case PARTICLE_MOVE_TYPE::SPREAD:
				{
					D3DXVECTOR3 dir = {
						cosf(D3DXToRadian(m_fAngle)),
						0.f,
						sinf(D3DXToRadian(m_fAngle))
					};

					particle.vVelocity = dir * randRange(3.0f, 4.0f);

					m_fAngle += (360.f / m_EmitCount);
					if (m_fAngle >= 360.f)
						m_fAngle = 0.f;
					break;
				}
				default:
					break;
				}
				particle.vPos = m_vPos; //������Ʈ ��ġ
				break; // �� ���� emit
			}
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
	// ������ �ؽ�ó RGB��
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	// ���Ĵ� �ؽ�ó �� ���� ����
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	// ī�޶� ����
	CCamera* pCamera = CCameraMgr::GetInstance()->Get_MainCamera();
	if (!pCamera) return;

	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	pDevice->SetTransform(D3DTS_WORLD, &identity); // ��� ��ƼŬ�� ���� ����

	D3DXVECTOR3 camPos = pCamera->Get_Eye();

	// �ؽ�ó �� FVF ����
	pDevice->SetTexture(0, m_pTexture->Get_Handle());
	pDevice->SetFVF(FVF_PARTICLE);

	// ���� ���� Lock
	VTXPARTICLE* pVertices = nullptr;
	if (FAILED(m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
		return;

	int vtxCount = 0;

	for (const auto& particle : m_vecParticles)
	{
		if (!particle.bActive) continue;

		const int maxVertexCount = 6 * m_iMaxParticles;
		if (vtxCount + 6 > maxVertexCount)
			break;

		const float half = particle.fSize * 0.5f;
		const _vec3& c = particle.vPos;

		// ������ ȸ�� ���� ���
		_vec3 vLook = camPos - c;
		D3DXVec3Normalize(&vLook, &vLook);

		_vec3 vUp(0.f, 1.f, 0.f);
		_vec3 vRight;
		D3DXVec3Cross(&vRight, &vUp, &vLook);
		D3DXVec3Normalize(&vRight, &vRight);

		// �ٽ� up ����
		_vec3 vTrueUp;
		D3DXVec3Cross(&vTrueUp, &vLook, &vRight);

		// ���� 6�� (�� �ﰢ��)
		pVertices[vtxCount++] = { c - vRight * half - vTrueUp * half,	particle.color ,{0.f, 1.f} };
		pVertices[vtxCount++] = { c - vRight * half + vTrueUp * half,	particle.color ,{0.f, 0.f} };
		pVertices[vtxCount++] = { c + vRight * half + vTrueUp * half,	particle.color ,{1.f, 0.f} };

		pVertices[vtxCount++] = { c - vRight * half - vTrueUp * half,	particle.color ,{0.f, 1.f} };
		pVertices[vtxCount++] = { c + vRight * half + vTrueUp * half,	particle.color ,{1.f, 0.f} };
		pVertices[vtxCount++] = { c + vRight * half - vTrueUp * half,	particle.color ,{1.f, 1.f} };
	}

	m_pVB->Unlock();

	// �׸���
	pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXPARTICLE));
	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, vtxCount / 3);

	// ���� ����
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pDevice->SetTexture(0, nullptr);
}

void CParticle::Render_Panel(ImVec2 size)
{
	ImGui::PushID(this); // ������ ID �ο�

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

		// �ؽ�ó ����
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
		ImGui::Separator();

		switch (m_eNowState)
		{
		case Engine::EMITTING:
			ImGui::Text("EMITTING");
			break;
		case Engine::STOPPING:
			ImGui::Text("STOPPING");
			break;
		case Engine::NONEACTIVE:
			ImGui::Text("NONEACTIVE");
			break;
		default:
			break;
		}
		ImGui::Separator();
		ImGui::Text("Emit Elapsed: %.2f", m_fElapsedEmit);


		ImGui::Text("Loop");
		ImGui::Checkbox("##Loop", &m_bLoop);
		if (!m_bLoop) {
			ImGui::SliderFloat("Emit Time", &m_fEmitLife, 0.1f, 20.f, "%.3f");
			if (m_eNowState == NONEACTIVE) {
				if (ImGui::Button("ReEmit")) {
					Request_Emit();
				}
			}
		}
		ImGui::Separator();


		ImGui::Text("Movement Type");
		ImGui::RadioButton("FIRE", &m_iMovementType, 0); ImGui::SameLine();
		ImGui::RadioButton("DUST", &m_iMovementType, 1);
		ImGui::RadioButton("BREATH", &m_iMovementType, 2);
		ImGui::RadioButton("SPREAD", &m_iMovementType, 3);
		ImGui::RadioButton("RADIAL", &m_iMovementType, 4);

		if (m_iMovementType == 3 || m_iMovementType == 4) {
			ImGui::Text("Radial Angle: %.2f", m_fAngle);
			ImGui::SliderInt("Emit Count : ", &m_EmitCount, 10, m_iMaxParticles, "%d");
		}
		ImGui::Separator();

		m_iPrevMaxParticles = m_iMaxParticles;
		ImGui::SliderInt("MaxParticles : ", &m_iMaxParticles, 40, 500, "%d");

		if (m_iPrevMaxParticles != m_iMaxParticles) {
			m_vecParticles.resize(m_iMaxParticles);
			m_iPrevMaxParticles = m_iMaxParticles;
			m_pDevice->CreateVertexBuffer(
				sizeof(VTXPARTICLE) * 6 * m_iMaxParticles,
				D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
				FVF_PARTICLE,
				D3DPOOL_DEFAULT,
				&m_pVB,
				nullptr);
		}
		ImGui::InputFloat3("Velocity", (_float*)&m_vVelocity, "%.2f");
		ImGui::InputFloat3("Axis", (_float*)&m_vAxis, "%.2f");
		ImGui::SliderFloat("SpawnInterval", &m_fSpawnInterval, 0.03f, 10.f, "%.3f");
		ImGui::SliderFloat("LifeTime", &m_fLifeTime, 0.1f, 20.f, "%.3f");
		ImGui::SliderFloat("Size", &m_fSize, 0.1f, 10.f, "%.3f");
		ImGui::SliderFloat("Speed", &m_fSpeed, 1.f, 10.f, "%.3f");

		ImGui::Separator();
		ImGui::InputFloat3("Offset", (_float*)&m_vOffset, "%.2f");

		// 1. float[4] ��ȯ
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

	ImGui::PopID(); // PushID�� ���� ����
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
	outJson["speed"] = m_fSpeed;

	outJson["loop"] = m_bLoop;
	outJson["emit_life"] = m_fEmitLife;
	outJson["emit_count"] = m_EmitCount;
	outJson["movement_type"] = m_iMovementType;

	outJson["velocity"] = { m_vVelocity.x, m_vVelocity.y, m_vVelocity.z };
	outJson["offset"] = { m_vOffset.x, m_vOffset.y, m_vOffset.z };

	outJson["base_color"] = {
		((BYTE)((m_BaseColor) >> 24) & 0xFF),  // A
		((BYTE)((m_BaseColor) >> 16) & 0xFF),  // R
		((BYTE)((m_BaseColor) >> 8) & 0xFF),   // G
		((BYTE)((m_BaseColor) >> 0) & 0xFF)    // B
	};

	outJson["Particle_Type"] = m_eMoveType;

	if (m_pTexture && !m_pTexture->GetKey().empty())
		outJson["texture_path"] = m_pTexture->GetKey();
}


void CParticle::Deserialize(const json& inJson)
{
	m_fSpawnInterval = inJson.value("spawn_interval", 1.0f);
	m_iMaxParticles = inJson.value("max_particles", 100);
	m_fLifeTime = inJson.value("life_time", 1.0f);
	m_fSize = inJson.value("size", 1.0f);
	m_fSpeed = inJson.value("speed", 1.0f);

	m_bLoop = inJson.value("loop", true);
	m_fEmitLife = inJson.value("emit_life", 3.5f);
	m_EmitCount = inJson.value("emit_count", 20);
	m_iMovementType = inJson.value("movement_type", 0);
	m_eMoveType = static_cast<PARTICLE_MOVE_TYPE>(inJson.value("Particle_Type", 0));

	auto vel = inJson.value("velocity", std::vector<float>{0.f, 0.f, 0.f});
	if (vel.size() == 3) m_vVelocity = { vel[0], vel[1], vel[2] };

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

void CParticle::Request_Emit()
{
	m_fElapsedEmit = 0;
	m_fElapsedTime = 0;
	m_eNowState = EMITTING;
	m_fAngle = 0;

	for (auto& p : m_vecParticles)
		p.bActive = false;
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

int CParticle::Particle_Count()
{
	int count = 0;
	for (auto& p : m_vecParticles) {
		if (p.bActive) ++count;
	}

	return count;
}

void CParticle::Set_Radial()
{
}

