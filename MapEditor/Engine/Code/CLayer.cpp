#include "Engine_Define.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CSceneMgr.h"

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

	// ������Ʈ ����Ʈ ��� (Ʈ�� ����)
	if (ImGui::TreeNode(("Objects (" + std::to_string(m_ObjectList.size()) + ")").c_str()))
	{
		for (auto* obj : m_ObjectList)
		{
			if (!obj) continue;

			string objID = "##" + obj->Get_InstanceName();
			ImGui::PushID(objID.c_str());

			bool isSelected = (CSceneMgr::GetInstance()->Get_SelectedObject() == obj);
			if (ImGui::Selectable(obj->Get_InstanceName().c_str(), isSelected))
			{
				CSceneMgr::GetInstance()->Set_SelectedObject(obj);
			}

			ImGui::PopID();
		}
		ImGui::TreePop();
	}

	ImGui::Separator();

	 //���õ� ������Ʈ�� ������ �г� ���
	if (CGameObject* selected = CSceneMgr::GetInstance()->Get_SelectedObject())
	{
			ImGui::PushID(selected);
			selected->Render_Panel();
			ImGui::PopID();
	}
}


HRESULT CLayer::Add_Object(CGameObject* object)
{
	//��ȸ�� ���� �˻� (���� �� ������ X)
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

//���ǿ� �´� ������Ʈ ����
void CLayer::Remove_Object(const string& tag)
{
	CGameObject* target = nullptr;

	//��ȸ�� ���� �˻�
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

_bool CLayer::Has_Object(const string& tag)
{
	//�ϴ��� ���̾� ���� ������Ʈ�� ���� �ʿ�� ���� ����
	//
	return false;
}

void CLayer::Serialize(json& jLayer) const
{
	jLayer["objects"] = json::array(); // ������ �迭�� �ʱ�ȭ (�� null �� ��)

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


void CLayer::Pop_Object(const string& tag)
{
	CGameObject* target = nullptr;

	//��ȸ�� ���� �˻�
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
