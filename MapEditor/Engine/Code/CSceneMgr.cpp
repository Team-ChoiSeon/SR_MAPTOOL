#include "Engine_Define.h"
#include "CSceneMgr.h"
#include "CScene.h"
#include "GUISystem.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CModel.h"
#include "CMesh.h"
#include "CMaterial.h"
#include "CCamera.h"
#include "CTexture.h"
#include "CFunction.h"
#include "CFactoryMgr.h"
#include "CResourceMgr.h"
//#include "CLight.h"

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
	
	return S_OK;
}

void CSceneMgr::Update_Scene(_float& dt)
{
	if (m_CurScene) {
		GUISystem::GetInstance()->RegisterPanel("SceneTag", [this]() {Render_SceneSelector();});
		GUISystem::GetInstance()->RegisterPanel("SceneLoad", [this]() {Save_LoadPanel();});
	
		m_CurScene->Update_Scene(dt);
	}
}

void CSceneMgr::LateUpdate_Scene(_float& dt)
{
	if (m_CurScene)
		m_CurScene->LateUpdate_Scene(dt);
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

	if (m_CurScene) {
		HRESULT exit = m_CurScene->Exit_Scene();
		if (FAILED(exit)) {
			MessageBoxW(0, L"씬 전환 실패, 씬 퇴장 오류", L"error", MB_OK);
		}
	}

	HRESULT enter = next->Enter_Scene();

	if (FAILED(enter)) {
		MessageBoxW(0, L"씬 전환 실패, 씬 입장 오류", L"error", MB_OK);
		return;
	}

	m_CurScene = next;
}

void CSceneMgr::Free()
{
	for (auto& Pair : m_SceneContainer) {
		Safe_Release(Pair.second);
	}
	m_SceneContainer.clear();
}

void CSceneMgr::Render_SceneSelector()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	ImGui::Begin("Scene_Select", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	const std::string& currentSceneName = m_SceneList[m_SceneIndex];

	const float comboHeight = 25.0f;     // 콤보 박스 닫힌 상태 높이
	const float itemHeight = 20.0f;      // 항목 하나의 높이

	// 콤보 박스 높이 키우기 (FramePadding을 이용)
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
	ImVec2 size = ImVec2(300.0f, 80.0f);
	ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(size, ImGuiCond_Always);
	ImGui::Begin("Scene_Saver", nullptr,
		ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_NoMove);

	ImGui::Spacing();

	// 저장 버튼
	if (ImGui::Button("Save"))
	{
		const wstring folder = GUISystem::GetInstance()->Open_FolderDialogue();
		if (!folder.empty()) {
			const std::string sceneName = m_CurScene->Get_Name();
			if (!sceneName.empty()) {
				std::wstring savePath = folder + L"/" + CFunction::toWString(sceneName) + L".json";
				Save_SceneToJson(savePath);
			}
			else {
				MessageBoxW(nullptr, L"씬 이름이 비어 있습니다", L"에러", MB_OK);
			}
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Open File")) {
		const wstring file = GUISystem::GetInstance()->Open_FileDialogue();
		Load_JsonToCScene(file);
	}

	ImGui::End();
}


void CSceneMgr::Save_SceneToJson(const std::wstring& path)
{
	json jScene;

	for (int i = 0; i < (int)LAYER_ID::L_END; ++i)
	{
		for (auto& obj : m_CurScene->Get_Layer(static_cast<LAYER_ID>(i))->Get_Object())
		{
			json jObj;

			jObj["Layer"] = m_CurScene->Layer_ToString(static_cast<LAYER_ID>(i));
			jObj["class"] = obj->Get_Name();
			jObj["name"] = obj->Get_InstanceName();

			// === Transform
			if (auto transform = obj->Get_Component<CTransform>())
			{
				auto pos = transform->Get_Pos();
				auto rot = transform->Get_Rotate();
				auto scale = transform->Get_Scale();
				auto pivot = transform->Get_Pivot();
				auto orbit = transform->Get_Orbit();
				auto parent = transform->Get_Parent();

				jObj["position"] = { pos.x, pos.y, pos.z };
				jObj["rotation"] = { rot.x, rot.y, rot.z };
				jObj["scale"] = { scale.x, scale.y, scale.z };
				jObj["pivot"] = { pivot.x, pivot.y, pivot.z };
				jObj["orbit"] = { orbit.x, orbit.y, orbit.z };
				jObj["parent_obj"] = parent ? parent->Get_Name() : "";
			}

			// === Model
			if (auto model = obj->Get_Component<CModel>())
			{
				if (auto mesh = model->Get_Mesh())
				{
					jObj["mesh"] = mesh->Get_Key();

					const auto& max = mesh->Get_AABBBOX().vMax;
					const auto& min = mesh->Get_AABBBOX().vMin;

					jObj["AABB"]["max"] = { max.x, max.y, max.z };
					jObj["AABB"]["min"] = { min.x, min.y, min.z };
				}

				if (auto mat = model->Get_Material())
				{
					jObj["matKey"] = mat->Get_Key();
					jObj["material"]["diffuse"] = mat->Get_Diffuse() ? mat->Get_Diffuse()->GetKey() : "";
					jObj["material"]["normal"] = mat->Get_Normal() ? mat->Get_Normal()->GetKey() : "";
					jObj["material"]["roughness"] = mat->Get_Roughness() ? mat->Get_Roughness()->GetKey() : "";
					jObj["material"]["specular"] = mat->Get_Specular() ? mat->Get_Specular()->GetKey() : "";
					jObj["material"]["emissive"] = mat->Get_Emissive() ? mat->Get_Emissive()->GetKey() : "";
				}
			}

			// === Camera
			if (auto cam = obj->Get_Component<CCamera>())
			{
				json jCam;

				jCam["fov"] = cam->Get_FOV();
				jCam["near"] = cam->Get_Near();
				jCam["far"] = cam->Get_Far();
				jCam["pitch"] = cam->Get_Pitch();
				jCam["yaw"] = cam->Get_Yaw();
				jCam["roll"] = cam->Get_Roll();

				jCam["eye"] = { cam->Get_Eye().x, cam->Get_Eye().y, cam->Get_Eye().z };
				jCam["lookDir"] = { cam->Get_Dir().x, cam->Get_Dir().y, cam->Get_Dir().z };
				jCam["up"] = { cam->Get_Up().x, cam->Get_Up().y, cam->Get_Up().z };

				jObj["camera"] = jCam;
			}

			// === 최종 오브젝트 push
			jScene["objects"].push_back(jObj);
		}
	}

	//  여기서 한 번만 저장
	std::string jsonText = jScene.dump(4); // UTF-8 문자열

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

void CSceneMgr::Load_JsonToCScene(const std::wstring& path)
{
	HANDLE hFile = ::CreateFileW(
		path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

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

	for (const auto& jObj : jScene["objects"])
	{
		string className = jObj["class"];
		string instanceName = jObj["name"];
		string layerName = jObj["Layer"];

		// 오브젝트 생성
		CGameObject* pObj = CFactoryMgr::Create(className);

		if (!pObj) {
			//MessageBoxW(0, L"프로토 매니저 등록 실패", L"Error", MB_OK);
			continue;
		}

		
		pObj->Set_Name(className);

		//Transform
		if (auto transform = pObj->Get_Component<CTransform>()) {
			auto pos = jObj["position"];
			auto rot = jObj["rotation"];
			auto scale = jObj["scale"];
			auto pivot = jObj["pivot"];
			auto orbit = jObj["orbit"];
		
			transform->Set_Pos({ pos[0], pos[1], pos[2] });
			transform->Set_Rotate({ rot[0], rot[1], rot[2] });
			transform->Set_Scale({ scale[0], scale[1], scale[2] });
			transform->Set_Pivot({ pivot[0], pivot[1], pivot[2] });
			transform->Set_Orbit({ orbit[0], orbit[1], orbit[2] });
			// 부모는 나중에 처리 (전체 파싱 후 연결)
		}
		
		// Model
		if (auto model = pObj->Get_Component<CModel>()) {
			// Model 키 존재 여부 확인
			if (jObj.contains("mesh") && jObj.contains("matKey")) {
				string meshKey = jObj["mesh"];
				string materialKey = jObj["matKey"];

				if (!meshKey.empty() && !materialKey.empty()) {
					model->Set_Model(meshKey, materialKey);

					if (auto mat = model->Get_Material()) {
						if (jObj.contains("material")) {
							auto& jMat = jObj["material"];

							mat->Set_Diffuse(CResourceMgr::GetInstance()->LoadTexture(jMat.value("diffuse", "")));
							mat->Set_Normal(CResourceMgr::GetInstance()->LoadTexture(jMat.value("normal", "")));
							mat->Set_Roughness(CResourceMgr::GetInstance()->LoadTexture(jMat.value("roughness", "")));
							mat->Set_Specular(CResourceMgr::GetInstance()->LoadTexture(jMat.value("specular", "")));
							mat->Set_Emissive(CResourceMgr::GetInstance()->LoadTexture(jMat.value("emissive", "")));
						}
					}
				}
			}

		}
		
		// Camera
		if (jObj.contains("camera")) {
			auto& jCam = jObj["camera"];
			if (auto cam = pObj->Get_Component<CCamera>()) {
				cam->Set_View(
					{ jCam["eye"][0], jCam["eye"][1], jCam["eye"][2] }, 
					{ jCam["up"][0], jCam["up"][1], jCam["up"][2] },
					{ jCam["lookDir"][0], jCam["lookDir"][1], jCam["lookDir"][2] });
				cam->Set_Proj(jCam["fov"], jCam["near"], jCam["far"]);
				cam->Set_YawPitchRoll({ jCam["yaw"], jCam["pitch"], jCam["roll"] });
			}
		}
		
		// 레이어에 추가
		LAYER_ID layerID = m_CurScene->String_ToLayer(layerName);
		m_CurScene->Get_Layer(layerID)->Add_Object(pObj);
	}
}


