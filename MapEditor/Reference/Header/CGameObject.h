#pragma once
#include "CBase.h"
#include "CComponent.h"

BEGIN(Engine)
static vector<string>ComponentList = { "Transform","Camera","Model","InputSystem","Light","RigidBody","Collider"};

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
	static CGameObject* Create(json jObj);
	static CGameObject* CreateDeafult();
	HRESULT Ready_GameObject(json jObj);
	void Update_GameObject(_float& dt);
	void LateUpdate_GameObject(_float& dt);
	void Render_Panel();
public:
	void Serialize(json& outJson) const;
	void Deserialize(const json& inJson);

	const string& Get_Name() { return ClassName; }
	void Set_InstanceName(const string& name) { InstanceName = name; }
	string Get_InstanceName();
	string Get_LayerName() { return LayerName; };
	void Set_LayerName(const string& layer) { LayerName = layer; };

	void Add_Component_ByName(const string& name);
	type_index typeid_from_string(const string& name);
public:
	template<typename T, typename ...Args>
	T* Add_Component(Args&& ...args);

	template<typename T>
	void TryLoadComponent(CGameObject* obj, const json& jComponents, const char* compName);

	template<typename T>
	T* Get_Component();

	template<typename T>
	void Remove_Component();

protected:
	string ClassName;
	string InstanceName;
	string LayerName;
	ImVec2 size{ 300, 0 };
	unordered_map<type_index, CComponent*> m_ComponentMap;

private:
	virtual void Free();
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
inline void CGameObject::TryLoadComponent(CGameObject* obj, const json& jComponents, const char* compName)
{
	if (jComponents.contains(compName)) {
		T* comp = obj->Add_Component<T>();
		comp->Deserialize(jComponents[compName]);//추후 추가
	}
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