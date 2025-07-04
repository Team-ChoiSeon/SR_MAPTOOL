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

void CLayer::Render_Panel()
{
		if (ImGui::CollapsingHeader(m_LayerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto* obj : m_ObjectList)
			{
				if (obj)
				{
					ImGui::PushID(obj); // ID 충돌 방지
					obj->Render_Panel(); // 오브젝트 개별 패널
					ImGui::PopID();
				}
			}
		}
}

HRESULT CLayer::Add_Object(CGameObject* object)
{
	//순회용 벡터 검색 (같은 것 있으면 X)
	auto iter = find_if(m_ObjectList.begin(), m_ObjectList.end(), [&object](CGameObject* list)->bool {
		return list == object;
		});

	if (iter != m_ObjectList.end()) {
		MessageBoxW(0, L"Object Instnace is Already Exist", L"Error", MB_OK);
		return E_FAIL;
	}

	m_ObjectList.push_back(object);
	return S_OK;
}

//조건에 맞는 오브젝트 삭제
void CLayer::Remove_Object(const string& tag)
{
	CGameObject* target = nullptr;

	//순회용 벡터 검색
	auto iter = find_if(m_ObjectList.begin(), m_ObjectList.end(),
		[&tag](CGameObject* data)->bool {
			return data->Get_Name() == tag;
		});

	if (iter != m_ObjectList.end()) {
		target = (*iter);
		m_ObjectList.erase(iter);
	}

	Safe_Release(target);
}

void CLayer::Serialize(json& jLayer) const
{
	jLayer["name"] = m_LayerName;
	for (CGameObject* obj : m_ObjectList)
	{
		if (!obj) continue;
		json jObj;
		obj->Serialize(jObj);
		jLayer["objects"].push_back(jObj);
	}
}

void CLayer::Deserialize(const json& jLayer)
{
	m_LayerName = jLayer.value("name", "Unnamed");

	for (const auto& jObj : jLayer["objects"])
	{
		string className = jObj["class"];
		CGameObject* obj = CGameObject::Create(jObj);
		if (!obj) continue;

		obj->Deserialize(jObj);
		Add_Object(obj);
	}
}

void CLayer::Pop_Object(const string& tag)
{
	CGameObject* target = nullptr;

	//순회용 벡터 검색
	auto iter = find_if(m_ObjectList.begin(), m_ObjectList.end(),
		[&tag](CGameObject* data)->bool {
			return data->Get_InstanceName() == tag;
		});

	if (iter != m_ObjectList.end()) {
		target = (*iter);
		m_ObjectList.erase(iter);
	}
}

HRESULT CLayer::Ready_Layer()
{
	//m_ObjectList.reserve(5);
	return S_OK;
}

void CLayer::Free()
{
	for (CGameObject*& obj : m_ObjectList) {
		Safe_Release(obj);
	}
	m_ObjectList.clear();
}
