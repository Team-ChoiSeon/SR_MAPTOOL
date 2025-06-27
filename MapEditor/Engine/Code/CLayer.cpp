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

void CLayer::Add_Object(CGameObject* object)
{
	auto iter = find_if(m_ObjectList.begin(), m_ObjectList.end(), [&object](CGameObject* list)->bool {
		return list == object;
		});

	if (iter == m_ObjectList.end())
		m_ObjectList.push_back(object);
}

void CLayer::Add_ObjectList(vector<CGameObject*> obj)
{
	for (CGameObject*& obj : obj) {
		Add_Object(obj);
	}
}

//조건에 맞는 오브젝트 추출
void CLayer::Remove_Object(function<bool(CGameObject*)> predicate)
{
	if (m_ObjectList.empty()) return;

	auto iter = find_if(m_ObjectList.begin(), m_ObjectList.end(), predicate);

	if (iter != m_ObjectList.end())
		m_ObjectList.erase(iter);
}

//조건에 맞는 벡터 추출
vector<CGameObject*> CLayer::Pop_Objects(function<bool(CGameObject*)> predicate)
{
	if (m_ObjectList.empty()) return vector<CGameObject*>();

	vector<CGameObject*> result;

	for (auto it = m_ObjectList.begin(); it != m_ObjectList.end(); )
	{
		if (predicate(*it))
		{
			result.push_back(*it);
			it = m_ObjectList.erase(it); // 삭제되므로 다음으로 이동
		}
		else {
			++it;
		}
	}

	return result;
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
}
