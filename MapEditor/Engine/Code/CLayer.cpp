#include "Engine_Define.h"
#include "CLayer.h"
#include "CGameObject.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

CLayer* CLayer::Create()
{
	CLayer* instance = new CLayer;

	if (FAILED(instance->Ready_Layer())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

void CLayer::Update_Layer(_float& dt)
{
	for (CGameObject*& obj : m_ObjectList) {
		obj->Update_GameObject(dt);
	}
}

void CLayer::LateUpdate_Layer(_float& dt)
{
	for (CGameObject*& obj : m_ObjectList) {
		obj->LateUpdate_GameObject(dt);
	}
}

void CLayer::Add_Object(const string& tag, CGameObject* object)
{
	//순회용 벡터 검색 (같은 것 있으면 X)
	auto iter = find_if(m_ObjectList.begin(), m_ObjectList.end(), [&object](CGameObject* list)->bool {
		return list == object;
		});

	//검색용 맵 검색 (같은 것 있으면 X)
	auto mapIter = m_ObjectMap.find(tag);

	if (iter != m_ObjectList.end()) {
		MessageBoxW(0, L"Object Instnace is Already Exist", L"Error", MB_OK);
		return;
	}

	if (mapIter != m_ObjectMap.end()) {
		MessageBoxW(0, L"Object Tag is Already Exist", L"Error", MB_OK);
		return;
	}

	m_ObjectList.push_back(object);
	m_ObjectMap.insert({ tag,object });
}

CGameObject* CLayer::Find_Object(const string& tag)
{
	auto mapIter = m_ObjectMap.find(tag);

	if (mapIter == m_ObjectMap.end()) {
		return nullptr;
	}

	return mapIter->second;
}


//조건에 맞는 오브젝트 삭제
void CLayer::Remove_Object(const string& tag)
{
	CGameObject* target = nullptr;

	auto mapIter = m_ObjectMap.find(tag);
	if (mapIter != m_ObjectMap.end()) {
		target = mapIter->second;
		m_ObjectMap.erase(mapIter);
	}

	if (!target)
		return;

	//순회용 벡터 검색 (같은 것 있으면 X)
	auto iter = find_if(m_ObjectList.begin(), m_ObjectList.end(), [&target](CGameObject* list)->bool {
		return list == target;
		});

	if (iter != m_ObjectList.end()) {
		m_ObjectList.erase(iter);
	}

	Safe_Release(target);
}


HRESULT CLayer::Ready_Layer()
{
	m_ObjectList.reserve(5);
	return S_OK;
}

void CLayer::Free()
{
	for (CGameObject*& obj : m_ObjectList) {
		Safe_Release(obj);
	}
	m_ObjectList.clear();
}
