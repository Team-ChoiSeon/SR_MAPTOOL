#include "Engine_Define.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CSceneMgr.h"
#include "CPickingMgr.h"

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
	if (m_ObjectList.empty())
		return;

	for (auto it = m_ObjectList.begin(); it != m_ObjectList.end(); )
	{
		CGameObject* obj = *it;
		if (!obj) {
			it = m_ObjectList.erase(it);
			continue;
		}

		string objID = "##" + obj->Get_InstanceName();
		ImGui::PushID(objID.c_str());

		bool isSelected = (CSceneMgr::GetInstance()->Get_SelectedObject() == obj
			&& obj->Get_LayerName() == this->m_LayerName);
		if (ImGui::Selectable(obj->Get_InstanceName().c_str(), isSelected))
		{
			CSceneMgr::GetInstance()->Set_SelectedObject(obj);
		}

		bool isDeleted = false;
		// 우클릭 팝업 메뉴
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
			{
				if (isSelected) {
					CSceneMgr::GetInstance()->Set_SelectedObject(nullptr);
				}
				Remove_ByIterator(it);
				isDeleted = true;

			}
			ImGui::EndPopup();   // 팝업은 바로 닫고
		}
		ImGui::PopID();

		if (!isDeleted)
			++it; // 삭제된 경우는 이미 it이 갱신됐으므로 증가하지 않음
	}

	ImGui::Separator();

	// 선택된 오브젝트의 패널 출력
	if (CGameObject* selected = CSceneMgr::GetInstance()->Get_SelectedObject())
	{
		if (selected->Get_LayerName() == this->m_LayerName) {
			ImGui::PushID(selected);
			selected->Render_Panel();
			ImGui::PopID();
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
			return data->Get_InstanceName() == tag;
		});

	if (iter != m_ObjectList.end()) {
		target = (*iter);
		m_ObjectList.erase(iter);
	}

	if (target == CSceneMgr::GetInstance()->Get_SelectedObject()) {
		CSceneMgr::GetInstance()->Set_SelectedObject(nullptr);
	}
	Safe_Release(target);
}

void CLayer::Remove_ByIterator(vector<CGameObject*>::iterator& iter)
{
	if (iter == m_ObjectList.end())
		return;

	CGameObject* target = *iter;

	// 먼저 선택 해제 (삭제되기 전 상태 유지)
	if (target == CSceneMgr::GetInstance()->Get_SelectedObject()) {
		CSceneMgr::GetInstance()->Set_SelectedObject(nullptr);
	}

	if (CPickingMgr::GetInstance()->Get_HittedObject() == target)
		CPickingMgr::GetInstance()->Clear_AllPickData();
	iter = m_ObjectList.erase(iter); // 벡터에서 제거

	if (target)
		Safe_Release(target); // 삭제
}


_bool CLayer::Has_Object(const string& tag)
{
	//일단은 레이어 속한 오브젝트만 보일 필요는 없어 보임
	//
	return false;
}

void CLayer::Serialize(json& jLayer) const
{
	jLayer["objects"] = json::array(); // 무조건 배열로 초기화 (비어도 null 안 됨)

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

	for (const auto& jObj : jLayer["objects"])
	{
		if (CGameObject* obj = CGameObject::Create(jObj)) {
			Add_Object(obj);
		}
	}
}


CGameObject* CLayer::Find_Object(const string& tag)
{
	CGameObject* target = nullptr;

	//순회용 벡터 검색
	auto iter = find_if(m_ObjectList.begin(), m_ObjectList.end(),
		[&tag](CGameObject* data)->bool {
			return data->Get_InstanceName() == tag;
		});

	if (iter != m_ObjectList.end()) {
		target = (*iter);
	}

	return target;
}

CGameObject* CLayer::Pop_Object(const string& tag)
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

	return target;
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
