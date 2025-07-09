#include "Engine_Define.h"
#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"
#include "GUISystem.h"
#include "CFunction.h"
#include "CPrefabMgr.h"
#include "CGameObject.h"
#include "CPickingMgr.h"
#include "CEditorSystem.h"

IMPLEMENT_SINGLETON(CSceneMgr)

CSceneMgr::CSceneMgr()
	:m_CurScene(nullptr), m_SceneIndex(0)
{
}

CSceneMgr::~CSceneMgr()
{
	Free();
}

HRESULT CSceneMgr::Ready_SceneMgr()
{
	Load_SceneData("../../data/SceneData");
	return S_OK;
}

void CSceneMgr::Update_Scene(_float& dt)
{
	if (m_CurScene) 
		m_CurScene->Update_Scene(dt);

}

void CSceneMgr::LateUpdate_Scene(_float& dt)
{
	if (m_CurScene)
		m_CurScene->LateUpdate_Scene(dt);
}

void CSceneMgr::Render_Scene()
{
	if (m_CurScene)
		m_CurScene->Render_Panel();

	Create_Object();

	Render_SceneSelector();
	Save_LoadPanel();
}

void CSceneMgr::Add_Scene(string sceneTag, CScene* scene)
{
	scene->Set_Name(sceneTag);
	m_SceneContainer.insert({ sceneTag,scene });
	m_SceneList.push_back(sceneTag);
}

void CSceneMgr::Change_Scene(string sceneTag)
{
	auto iter = m_SceneContainer.find(sceneTag);

	if (iter == m_SceneContainer.end()) return;

	CScene* next = iter->second;

	m_CurScene = next;
}

void CSceneMgr::Free()
{
	for (auto& Pair : m_SceneContainer) {
		Safe_Release(Pair.second);
	}
	m_SceneContainer.clear();
	m_SceneList.clear();
}

bool CSceneMgr::Is_ValidObject(CGameObject* obj)
{
	if (!obj || !m_CurScene)
		return false;

	const string& layerName = obj->Get_LayerName();

	CLayer* layer = m_CurScene->Get_Layer(layerName);
	if (!layer)
		return false;

	const auto& objs = layer->Get_Object();
	return find(objs.begin(), objs.end(), obj) != objs.end();
}


void CSceneMgr::Load_SceneData(const string& scenePath)
{
	namespace fs = std::filesystem;

	m_SceneContainer.clear();
	m_SceneList.clear();
	string err;

	try {
		for (const auto& entry : fs::directory_iterator(scenePath))
		{
			if (!entry.is_regular_file()) continue;

			const fs::path& filePath = entry.path();
			if (filePath.extension() != ".json") continue;

			ifstream inFile(filePath);
			if (!inFile.is_open()) continue;

			json jScene;
			inFile >> jScene;

			if (!jScene.contains("scene_name")) continue;

			string sceneName = filePath.stem().string();
			err = sceneName;

			CScene* newScene = CScene::Create();
			newScene->Deserialize(jScene);
			m_SceneContainer[sceneName] = newScene;
			m_SceneList.push_back(sceneName);
			if (m_CurScene == nullptr) m_CurScene = newScene; //초기 로딩 시, 씬 넣기
		}
	}
	catch (const std::exception& e) {
		err += "loading err";
		MessageBox(0, err.c_str(), "Scene", MB_OK);
	}
}

void CSceneMgr::Render_SceneSelector()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	ImGui::Begin("Scene_Select", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	const std::string& currentSceneName = m_SceneList[m_SceneIndex];

	const float comboHeight = 25.0f;     // 콤보 박스 닫힌 상태 높이
	const float itemHeight = 20.0f;      // 항목 하나의 높이

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, (comboHeight - ImGui::GetFontSize()) * 0.5f));

	if (ImGui::BeginCombo("##SceneCombo", currentSceneName.c_str()))
	{
		for (int i = 0; i < m_SceneList.size(); ++i)
		{
			bool is_selected = (m_SceneIndex == i);
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec2 size = ImVec2(0.0f, itemHeight);  // 세로 크기 고정

			// 배경 선택 처리
			if (ImGui::Selectable(("##" + std::to_string(i)).c_str(), is_selected, 0, size))
			{
				if (!is_selected)
				{
					m_SceneIndex = i;
					Change_Scene(m_SceneList[i]);
				}
			}

			// 텍스트 수직 중앙 정렬
			const char* label = m_SceneList[i].c_str();
			ImVec2 textSize = ImGui::CalcTextSize(label);
			ImVec2 textPos = ImVec2(pos.x + 5.0f, pos.y + (itemHeight - textSize.y) * 0.5f);

			ImGui::GetWindowDrawList()->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), label);
		}
		ImGui::EndCombo();
	}

	ImGui::PopStyleVar(); // FramePadding 복원
	ImGui::End();
}

void CSceneMgr::Save_LoadPanel()
{
	ImVec2 pos = ImVec2(WINCX - 600, 10.0f);
	ImVec2 size = ImVec2(350.0f, 100.0f); 
	ImGui::SetNextWindowPos(pos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(size, ImGuiCond_Always);
	ImGui::Begin("Scene_Saver", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse);

	ImGui::Spacing();

	// === 씬 이름 입력 ===
	static char sceneNameBuf[128] = {};

	// 현재 씬 이름을 버퍼에 복사 (한 번만)
	if (sceneNameBuf[0] == '\0' && m_CurScene)
	{
		string curName = m_CurScene->Get_Name();
		strcpy_s(sceneNameBuf, curName.c_str());
	}

	ImGui::InputText("##Scene Name", sceneNameBuf, IM_ARRAYSIZE(sceneNameBuf));
	ImGui::SameLine();
	if (ImGui::Button("Rename ##Scene") && m_CurScene)
	{
		string newName = sceneNameBuf;
		m_CurScene->Set_Name(newName);
		MessageBoxW(nullptr, L"씬 이름이 변경되었습니다.", L"알림", MB_OK);
	}

	ImGui::Spacing();

	// === 저장 버튼 ===
	if (ImGui::Button("Save"))
	{
		const wstring folder = GUISystem::GetInstance()->Open_FolderDialogue();
		if (!folder.empty()) {
			const string sceneName = m_CurScene->Get_Name();
			if (!sceneName.empty()) {
				wstring savePath = folder + L"/" + CFunction::toWString(sceneName) + L".json";
				SerializeScene(savePath);
			}
			else {
				MessageBoxW(nullptr, L"씬 이름이 비어 있습니다", L"에러", MB_OK);
			}
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Open File")) {
		const wstring file = GUISystem::GetInstance()->Open_FileDialogue();
		DeSerializeScene(file);
	}

	ImGui::End();
}


void CSceneMgr::SerializeScene(const wstring& path)
{
	if (!m_CurScene)
	{
		MessageBoxW(nullptr, L"저장할 씬이 존재하지 않습니다", L"Error", MB_OK);
		return;
	}

	// 1. 씬 직렬화
	json jScene;
	m_CurScene->Serialize(jScene);
	//jScene = SortJsonKeys(jScene);
	// 2. JSON 문자열로 변환 (들여쓰기 4칸)
	string jsonText = jScene.dump(4); // UTF-8 문자열

	// 3. 파일로 저장
	HANDLE hFile = ::CreateFileW(
		path.c_str(),
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD written = 0;
		::WriteFile(hFile, jsonText.data(), (DWORD)jsonText.size(), &written, NULL);
		::CloseHandle(hFile);
	}
	else
	{
		MessageBoxW(nullptr, L"파일 저장 실패", L"Error", MB_OK);
	}
}

void CSceneMgr::DeSerializeScene(const wstring& path)
{
	HANDLE hFile = ::CreateFileW(
		path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBoxW(nullptr, L"파일 열기 실패", L"Error", MB_OK);
		return;
	}

	DWORD fileSize = GetFileSize(hFile, NULL);
	std::string jsonText(fileSize, '\0');
	DWORD bytesRead = 0;
	ReadFile(hFile, jsonText.data(), fileSize, &bytesRead, NULL);
	CloseHandle(hFile);

	json jScene = json::parse(jsonText);
	std::string sceneName = jScene.value("scene_name", "");

	if (!sceneName.empty()) {
		CScene* pScene = CScene::Create();
		pScene->Set_Name(sceneName);
		pScene->Deserialize(jScene);
		Add_Scene(sceneName, pScene);
		Change_Scene(sceneName); // 바로 전환까지
	}
	else {
		MessageBoxW(nullptr, L"scene_name이 없습니다", L"Error", MB_OK);
	}
}

void CSceneMgr::Create_Object()
{
	// 화면 아무 곳에서든 우클릭 시 팝업 열기
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !ImGui::IsAnyItemHovered())
	{
		ImGui::OpenPopup("GlobalContextMenu");
	}

	// 팝업 본체
	if (ImGui::BeginPopup("GlobalContextMenu"))
	{
		for (string name : CPrefabMgr::GetInstance()->Get_PrefabNames()) {
			if (ImGui::MenuItem(name.c_str())) {
				CGameObject* instance = CPrefabMgr::GetInstance()->Instantiate(name);
				CLayer* layer = m_CurScene->Get_Layer("DEFAULT");
				layer->Add_Object(instance);
				instance->Set_LayerName("DEFAULT");
			}
		}
		ImGui::EndPopup();
	}
}

bool CSceneMgr::Is_ObjectNameExist(const string& name) const {
	for (auto& pair : m_CurScene->Get_Layers()) {
		if (pair.second->Find_Object(name) != nullptr) {
			return true;
		}
	}
	return false;
}

void CSceneMgr::Set_SelectedObject(CGameObject* obj)
{
	selectObj = obj;
	CPickingMgr::GetInstance()->Set_PickedObj(obj);
	CEditorSystem::GetInstance()->Set_Select(obj);
}

json CSceneMgr::SortJsonKeys(const json& j)
{
	if (j.is_object()) {
		map<string, json> ordered(j.begin(), j.end());
		json result;
		for (const auto& [key, value] : ordered)
			result[key] = SortJsonKeys(value);
		return result;
	}
	else if (j.is_array()) {
		json result = json::array();
		for (const auto& item : j)
			result.push_back(SortJsonKeys(item));
		return result;
	}
	else {
		return j;
	}
}
