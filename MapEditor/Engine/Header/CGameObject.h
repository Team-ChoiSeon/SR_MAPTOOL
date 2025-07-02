#pragma once
#include "CBase.h"
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject :
	public CBase
{
protected:
	explicit CGameObject();
	virtual ~CGameObject() override;
public:
	CGameObject(const CGameObject&) = delete;
	CGameObject& operator=(const CGameObject&) = delete;

public:
	virtual HRESULT Ready_GameObject() PURE;
	virtual void Update_GameObject(_float& dt) PURE;
	virtual void LateUpdate_GameObject(_float& dt) PURE;
	const string& Get_Name() { return ObjectName; }
	LAYER_ID Get_LayerID() { return m_Layer; }

	void Set_Name(const string& name) { ObjectName = name; }
	void Set_LayerID(LAYER_ID layer) { m_Layer = layer; }

protected:
	void Update_Component(_float& dt);
	void LateUpdate_Component(_float& dt);
	void Release_Component();

public:
	template<typename T, typename ...Args>
	T* Add_Component(Args&& ...args);

	template<typename T>
	T* Get_Component();

	template<typename T>
	void Remove_Component();

private:
	string ObjectName;
	LAYER_ID m_Layer;
	unordered_map<type_index, CComponent*> m_ComponentMap;
private:
	virtual void Free() = 0;
};

template<typename T, typename ...Args>
inline T* CGameObject::Add_Component(Args && ...args)
{
	if (Get_Component<T>() != nullptr) //우선 가진 컴포넌트 검색
		return nullptr;

	T* comPtr = T::Create(forward<Args>(args)...);
	comPtr->m_pOwner = this;
	m_ComponentMap.insert({ type_index(typeid(T)),comPtr });

	return comPtr;

}

template<typename T>
inline T* CGameObject::Get_Component()
{
	auto iter = m_ComponentMap.find(type_index(typeid(T)));

	if (iter == m_ComponentMap.end())
		return nullptr;

	return static_cast<T*>(iter->second);
}

template<typename T>
inline void CGameObject::Remove_Component()
{
	// 일반 단일 컴포넌트 처리
	auto iter = m_ComponentMap.find(type_index(typeid(T)));

	if (iter == m_ComponentMap.end())
		return;

	T* target = static_cast<T*>(iter->second);

	m_ComponentMap.erase(iter);
	Safe_Release(target);
}


END