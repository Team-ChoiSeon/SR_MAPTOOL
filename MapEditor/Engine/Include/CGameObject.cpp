#include "Engine_Define.h"
#include "CGameObject.h"

CGameObject::CGameObject()
{
}

CGameObject::~CGameObject()
{
}


void CGameObject::Update_Component(_float& dt)
{
	for (auto& pair : m_ComponentMap)
		pair.second->Update_Component(dt);
}

void CGameObject::LateUpdate_Component(_float& dt)
{
	for (auto& pair : m_ComponentMap)
		pair.second->LateUpdate_Component(dt);
}

void CGameObject::Release_Component()
{
	for (auto& pair : m_ComponentMap)
		Safe_Release(pair.second);
}
