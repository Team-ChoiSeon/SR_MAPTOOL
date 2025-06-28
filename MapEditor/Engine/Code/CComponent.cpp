#include "Engine_Define.h"
#include "CComponent.h"

HRESULT CComponent::Ready_Component()
{
	return E_NOTIMPL;
}

void CComponent::Update_Component(_float& dt)
{
}

void CComponent::LateUpdate_Component(_float& dt)
{
}

CComponent* CComponent::Clone() const
{
	return nullptr;
}
