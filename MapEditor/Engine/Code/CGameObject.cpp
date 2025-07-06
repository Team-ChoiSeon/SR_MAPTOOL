#include "Engine_Define.h"
#include "CGameObject.h"
#include "CSceneMgr.h"
#include "CFunction.h"
#include "CTransform.h"
#include "CModel.h"
#include "CCamera.h"
#include "CPickable.h"
#include "CInputSystem.h"
#include "CLight.h"

#include "Cscene.h"

CGameObject::CGameObject()
{
}

CGameObject::~CGameObject()
{
}

CGameObject* CGameObject::Create(json jObj)
{
	CGameObject* instance = new CGameObject();

	if (FAILED(instance->Ready_GameObject(jObj))) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

CGameObject* CGameObject::CreateDeafult()
{
	CGameObject* instance = new CGameObject();
	return instance;
}

HRESULT CGameObject::Ready_GameObject(json jObj)
{
	Deserialize(jObj);
	return S_OK;
}

string CGameObject::Get_InstanceName()
{
	return InstanceName;
}

void CGameObject::Update_GameObject(_float& dt)
{
	for (auto& pair : m_ComponentMap) {
		if (pair.second->Get_ComponentActive())
			pair.second->Update_Component(dt);
	}

	for (auto it = m_ComponentMap.begin(); it != m_ComponentMap.end(); )
	{
		if (it->second && it->second->Get_ComponentDelete()) {
			CComponent* comp = it->second;
			it = m_ComponentMap.erase(it); 
			Safe_Release(comp);
		}
		else {
			++it;
		}
	}

}

void CGameObject::LateUpdate_GameObject(_float& dt)
{
	for (auto& pair : m_ComponentMap)
		if (pair.second->Get_ComponentActive())
			pair.second->LateUpdate_Component(dt);
}

void CGameObject::Render_Panel()
{
	ImVec2 windowPos = ImVec2(WINCX - 400, 20);
	ImVec2 windowSize = ImVec2(size);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

	if (ImGui::Begin("Object Editor", nullptr, ImGuiWindowFlags_None))
	{
		// 레이어 드롭다운
		const vector<string>& layerList = CSceneMgr::GetInstance()->Get_NowScene()->Get_LayerNames(); // 이제 string
		static int currentLayerIndex = 0;

		for (int i = 0; i < layerList.size(); ++i) {
			if (layerList[i] == LayerName) {
				currentLayerIndex = i;
				break;
			}
		}
		ImGui::Text("LAYER"); ImGui::SameLine();
		const char* currentPreview = layerList[currentLayerIndex].c_str();
		if (ImGui::BeginCombo("##Layer", currentPreview)) {
			for (int i = 0; i < layerList.size(); ++i) {
				bool isSelected = (i == currentLayerIndex);

				if (ImGui::Selectable(layerList[i].c_str(), isSelected)) {
					currentLayerIndex = i;
					CSceneMgr::GetInstance()->Get_NowScene()->SwapLayer(this, LayerName, layerList[i]);
					LayerName = layerList[i]; // 반영
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Separator();

		// 클래스명 (읽기 전용)
		ImGui::Text("Class: %s", ClassName.c_str());

		// 인스턴스 이름 (수정 가능)
		char nameBuf[64];
		strcpy_s(nameBuf, InstanceName.c_str());
		if (ImGui::InputText("##Instance Name", nameBuf, IM_ARRAYSIZE(nameBuf))) {
			InstanceName = nameBuf;
		}
	}


	for (auto& pair : m_ComponentMap) {
		pair.second->Render_Panel(size);
	}

	ImGui::Separator();
	ImGui::Text("Add Component");
	static int currentCompIndex = 0;
	vector<string> FilteredList;

	// 필터링: 이미 추가된 컴포넌트는 제외
	for (const auto& compName : ComponentList) {
		auto iter = m_ComponentMap.find(typeid_from_string(compName));
		if (iter == m_ComponentMap.end())
			FilteredList.push_back(compName);
	}
	
	if (FilteredList.empty()) {
		ImGui::TextDisabled("No more components to add.");
	}
	else {
		if (currentCompIndex >= FilteredList.size())
			currentCompIndex = 0;
	}
	if (ImGui::BeginCombo("##AddComponent",
		FilteredList.empty() ? "No components" : FilteredList[currentCompIndex].c_str()))
	{
	const char* compPreview = FilteredList[currentCompIndex].c_str();
		for (int i = 0; i < FilteredList.size(); ++i) {
			bool isSelected = (currentCompIndex == i);
			if (ImGui::Selectable(FilteredList[i].c_str(), isSelected)) {
				currentCompIndex = i;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	if (ImGui::Button("Add")) {
		const string& compName = FilteredList[currentCompIndex];
		Add_Component_ByName(compName);
	}
	ImGui::End();
}

void CGameObject::Serialize(json& outJson) const
{
	// 기본 정보
	outJson["class"] = ClassName;
	outJson["name"] = InstanceName;
	outJson["Layer"] = LayerName;

	// 컴포넌트 시리얼라이즈
	json jComponents;

	for (const auto& pair : m_ComponentMap)
	{
		const CComponent* comp = pair.second;
		if (!comp)
			continue;
		// 컴포넌트 이름 추출
		json jComp;
		comp->Serialize(jComp);
		string compName = comp->Get_ComponentName();
		jComponents[compName] = jComp;
	}

	outJson["components"] = jComponents;
}

void CGameObject::Deserialize(const json& inJson)
{
	ClassName = inJson["class"];
	InstanceName = inJson["name"];
	LayerName= inJson["Layer"] ;

	if (inJson.contains("components") && inJson["components"].is_object())
	{
		const json& jComps = inJson["components"];

		TryLoadComponent<CTransform>(this, jComps, "CTransform");
		TryLoadComponent<CCamera>(this, jComps, "CCamera");
		TryLoadComponent<CModel>(this, jComps, "CModel");
		TryLoadComponent<CLight>(this, jComps, "CLight");
		//===맵툴용 필수 컴포넌트====/
		Add_Component<CPickable>();
	}
}

void CGameObject::Add_Component_ByName(const string& name) {
	if (name == "Transform") Add_Component<CTransform>();
	else if (name == "Model") Add_Component<CModel>();
	else if (name == "Camera") Add_Component<CCamera>();
	else if (name == "InputSystem") Add_Component<CInputSystem>();
	else if (name == "Light") Add_Component<CLight>();
}

type_index CGameObject::typeid_from_string(const string& name)
{
	if (name == "Transform") return typeid(CTransform);
	if (name == "Model") return typeid(CModel);
	if (name == "Camera") return typeid(CCamera);
	if (name == "InputSystem") return typeid(CInputSystem);
	if (name == "Light") return typeid(CLight);

	return typeid(void); // fallback
}

void CGameObject::Free()
{
	for (auto& pair : m_ComponentMap)
		Safe_Release(pair.second);
}

