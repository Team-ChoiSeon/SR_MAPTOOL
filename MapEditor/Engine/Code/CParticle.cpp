#include "Engine_Define.h"
#include "CParticle.h"

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
	return S_OK;
}

void CParticle::Update_Component(_float& dt)
{
}

void CParticle::LateUpdate_Component(_float& dt)
{
}

void CParticle::Render_Particle(LPDIRECT3DDEVICE9 pDevice)
{
}

void CParticle::Render_Panel(ImVec2 size)
{
}

CComponent* CParticle::Clone() const
{
	return nullptr;
}

void CParticle::Serialize(json& outJson) const
{
	outJson["effect"] = m_strEffectName;
	outJson["loop"] = m_bLoop;
	outJson["duration"] = m_fDuration;
	outJson["spawn_rate"] = m_fSpawnRate;
	outJson["life_time"] = m_fLifeTime;
	outJson["start_Scale"] = { m_vStartScale.x,m_vStartScale.y,m_vStartScale.z };
	outJson["end_scale"] = { m_vEndScale.x, m_vEndScale.y, m_vEndScale.z };
}

void CParticle::Deserialize(const json& inJson)
{
	m_strEffectName = inJson.value("effect", "Fire");
	m_bLoop = inJson.value("loop", true);
	m_fDuration = inJson.value("duration", 3.f);
	m_fSpawnRate = inJson.value("spawn_rate", 10.f);
	m_fLifeTime = inJson.value("life_time", 1.f);

	auto s = inJson["start_scale"];
	m_vStartScale = { s[0], s[1], s[2] };

	auto e = inJson["end_scale"];
	m_vEndScale = { e[0], e[1], e[2] };
}

string CParticle::Get_ComponentName() const
{
	return "CParticle";
}

void CParticle::Free()
{
}
