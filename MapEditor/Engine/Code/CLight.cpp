#include "Engine_Define.h"
#include "CLight.h"


CLight::CLight()
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
	return nullptr;
}

HRESULT CLight::Ready_Component()
{
	return E_NOTIMPL;
}

void CLight::Update_Component(_float& dt)
{
}

void CLight::LateUpdate_Component(_float& dt)
{
}

void CLight::Render_Panel(ImVec2 size)
{
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

void CLight::Free()
{
}