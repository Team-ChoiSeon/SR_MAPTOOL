#include "Engine_Define.h"
#include "CComponent.h"

HRESULT CComponent::Ready_Component()
{
	return E_NOTIMPL;
}

void CComponent::Update_Component(float& dt)
{
}

void CComponent::LateUpdate_Component(float& dt)
{
}

CComponent* CComponent::Clone() const
{
	return nullptr;
}
