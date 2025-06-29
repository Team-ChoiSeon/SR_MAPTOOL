#include "pch.h"
#include "Engine_Define.h"
#include "CTestScene.h"
#include "CLayer.h"

#include "CGameObject.h"
#include "CTestCube.h"

#include "CCamera.h"
#include "CTransform.h"
#include "CCameraActor.h"
#include "CCameraMgr.h"

#include "GUISystem.h"

CTestScene::CTestScene()
	:pTarget(nullptr)
{
}

CTestScene::~CTestScene()
{
}

CTestScene* CTestScene::Create()
{
	CTestScene* instance = new CTestScene;

	if (FAILED(instance->Ready_Scene())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CTestScene::Ready_Scene()
{
	Create_Layer("Camera");
	Create_Layer("Object");

	m_mapLayer["Camera"]->Add_Object("Camera01", CCameraActor::Create());
	m_mapLayer["Object"]->Add_Object("Object01", CTestCube::Create());
	m_mapLayer["Object"]->Add_Object("Object02", CTestCube::Create());

	CCamera* cam = (m_mapLayer["Camera"]->Find_Object("Camera01"))->Get_Component<CCamera>();
	CCameraMgr::GetInstance()->Set_MainCamera(cam);

	return S_OK;
}


void CTestScene::Update_Scene(_float& dt)
{
	m_mapLayer["Camera"]->Update_Layer(dt);
	m_mapLayer["Object"]->Update_Layer(dt);

	GUISystem::GetInstance()->RegisterPanel("ObjectList", [this]() {Show_ObjectList();});

	if(pTarget)
		GUISystem::GetInstance()->RegisterPanel("ObjectEditor", [this]() {Edit_Object(pTarget);});
}

void CTestScene::LateUpdate_Scene(_float& dt)
{
	m_mapLayer["Camera"]->LateUpdate_Layer(dt);
	m_mapLayer["Object"]->LateUpdate_Layer(dt);
}

void CTestScene::Edit_Object(CGameObject* obj)
{
	ImVec2 windowSize = ImGui::CalcTextSize("Object Editor");
	windowSize.x += 250; // 예상 너비 추가

	ImVec2 pos = ImVec2(WINCX - windowSize.x - 150, 10);
	ImGui::SetNextWindowPos(pos, ImGuiCond_Once);

	ImGui::Begin("Object Editor", nullptr,ImGuiWindowFlags_AlwaysAutoResize);

	CTransform* transform = obj->Get_Component<CTransform>();
	_vec3 scale = transform->Get_Scale();
	_vec3 rotate = transform->Get_Rotate();

	ImGui::SliderFloat3("Scale", (float*)&scale, 0.0f, 5.0f);

	ImGui::SliderFloat3("Rotate", (float*)&rotate, 0.0f, 360.0f);

	transform->Set_Scale({ scale });
	transform->Set_Rotate({ rotate });
	ImGui::End();
}

void CTestScene::Show_ObjectList()
{
	if (ImGui::BeginListBox("Object List", ImVec2(250, 300)))
	{
		for (const auto& item : m_mapLayer["Object"]->Get_Object())
		{
			bool is_selected = (pTarget == item);
			if (ImGui::Selectable(item->Get_Name().c_str(), is_selected))
			{
				pTarget = item;
			}
		}
		ImGui::EndListBox();
	}
}

void CTestScene::Free()
{
	Free_AllLayer();
}

HRESULT CTestScene::Enter_Scene()
{
	return S_OK;
}

HRESULT CTestScene::Exit_Scene()
{
	return S_OK;
}
