#include "Engine_Define.h"
#include "CGameObject.h"
#include "CFunction.h"

CGameObject::CGameObject()
{
}

CGameObject::~CGameObject()
{
}

string CGameObject::Get_InstanceName()
{
	//string m = this->Get_Name() + to_string(CFunction::Find_ObjectIndex(this->Get_Name()));
	return this->Get_Name() + to_string(ObjCount);
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
