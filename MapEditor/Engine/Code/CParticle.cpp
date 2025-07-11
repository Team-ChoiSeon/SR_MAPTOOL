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

void CParticle::Render_Panel(ImVec2 size)
{
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

void CParticle::Free()
{
}
