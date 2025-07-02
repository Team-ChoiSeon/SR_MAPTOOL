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
#include "CPickingMgr.h"
#include "GUISystem.h"
#include "CFunction.h"

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
	Init_Layer();
	Add_Object( LAYER_ID::L_CAMERA, CCameraActor::Create());
	Add_Object(LAYER_ID::L_OBJECT, CTestCube::Create());

	CCamera* cam = (m_mapLayer[LAYER_ID::L_CAMERA]->Find_Object("Camera0"))->Get_Component<CCamera>();
	CCameraMgr::GetInstance()->Set_MainCamera(cam);

	return S_OK;
}


void CTestScene::Update_Scene(_float& dt)
{

	for (auto& pair : m_mapLayer) {
		pair.second->Update_Layer(dt);
	}
	GUISystem::GetInstance()->RegisterPanel("Object_Create", [this]() {Create_Object();});

	pTarget = CPickingMgr::GetInstance()->Get_PickedObj();
	GUISystem::GetInstance()->RegisterPanel("ObjectList", [this]() {Show_ObjectList();});

	if (pTarget)
		GUISystem::GetInstance()->RegisterPanel("ObjectEditor", [this]() {Edit_Object(pTarget);});
}

void CTestScene::LateUpdate_Scene(_float& dt)
{
	for (auto& pair : m_mapLayer) {
		pair.second->LateUpdate_Layer(dt);
	}
}

void CTestScene::Edit_Object(CGameObject* obj)
{
	if (obj == nullptr)
		return;

	CTransform* transform = obj->Get_Component<CTransform>();
	if (transform == nullptr)
		return;

	// â ��ġ�� ũ�� ���� ����
	ImVec2 windowPos = ImVec2(WINCX - 400, 20);
	ImVec2 windowSize = ImVec2(200, 200);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

	if (ImGui::Begin("Object Editor", nullptr, ImGuiWindowFlags_None))
	{
		ImGui::Text(obj->Get_Name().c_str());
		ImGui::Dummy(ImVec2(0, 2)); // 5�ȼ� ���� ����
		// ������
		_vec3 scale = transform->Get_Scale();
		float sx = scale.x, sy = scale.y, sz = scale.z;
		ImGui::Text("Scale");
		ImGui::Text("X"); ImGui::SameLine();
		ImGui::InputFloat("##Xscale", &sx, 0.1f, 1.0f, "%.2f");

		ImGui::Text("Y"); ImGui::SameLine();
		ImGui::InputFloat("##Yscale", &sy, 0.1f, 1.0f, "%.2f");

		ImGui::Text("Z"); ImGui::SameLine();
		ImGui::InputFloat("##Zscale", &sz, 0.1f, 1.0f, "%.2f");

		transform->Set_Scale({ sx, sy, sz });

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// ȸ��
		_vec3 rotate = transform->Get_Rotate();
		float rx = rotate.x, ry = rotate.y, rz = rotate.z;

		ImGui::Text("Rotate");
		ImGui::Text("X"); ImGui::SameLine();
		ImGui::InputFloat("##XRotate", &rx, 0.1f, 1.0f, "%.2f");

		ImGui::Text("Y"); ImGui::SameLine();
		ImGui::InputFloat("##YRotate", &ry, 0.1f, 1.0f, "%.2f");

		ImGui::Text("Z"); ImGui::SameLine();
		ImGui::InputFloat("##ZRotate", &rz, 0.1f, 1.0f, "%.2f");

		transform->Set_Rotate({ rx, ry, rz });

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		_vec3 position = transform->Get_Pos();
		float px = position.x, py = position.y, pz = position.z;

		ImGui::Text("Position");
		ImGui::Text("X"); ImGui::SameLine();
		ImGui::InputFloat("##XPosition", &px, 0.1f, 1.0f, "%.2f");

		ImGui::Text("Y"); ImGui::SameLine();
		ImGui::InputFloat("##YPosition", &py, 0.1f, 1.0f, "%.2f");

		ImGui::Text("Z"); ImGui::SameLine();
		ImGui::InputFloat("##ZPosition", &pz, 0.1f, 1.0f, "%.2f");

		transform->Set_Pos({ px, py, pz });

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// �巹�̾� ��Ӵٿ�
		ImGui::Text("Layer");

		const char* layerNames[] = { "Deafult", "Camera", "Object","Player" ,"Tile" };
		int currentLayer = static_cast<int>(obj->Get_LayerID());
		int newLayer = currentLayer;

		if (ImGui::Combo("##LayerCombo", &newLayer, layerNames, IM_ARRAYSIZE(layerNames)))
		{
			if (newLayer != currentLayer) // ���� ���̾��� ��� ����
			{
				LAYER_ID prevLayer = static_cast<LAYER_ID>(currentLayer);
				LAYER_ID nextLayer = static_cast<LAYER_ID>(newLayer);

				// ���ο� ���̾ �߰�
				if (FAILED( nextLayer,obj))
				{
					return;
				}
				else
				{
					m_mapLayer[prevLayer]->Pop_Object(obj->Get_InstanceName());
				}
			}
		}

		ImGui::End();
	}
}

void CTestScene::Show_ObjectList()
{
	ImVec2 windowSize = ImVec2(250, WINCY);
	ImVec2 windowPos = ImVec2(WINCX - 250, 0);

	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);

	if (ImGui::Begin("Object List", nullptr, ImGuiWindowFlags_None))
	{
		for (const auto& pair : m_mapLayer)
		{
			CLayer* pLayer = pair.second;
			if (pLayer->Get_Object().empty())
				continue; // ���̾ ������Ʈ�� ������ �ǳʶ�

			// ���̾� �̸��� ���� ID ����
			std::string layerName = Layer_ToString(pair.first);
			std::string label = "Layer: " + layerName;

			// ���̾� ���� ����
			ImGui::Dummy(ImVec2(0, 8));

			// ���̾� �̸� ���� �۾�
			ImGui::PushFont(GUISystem::GetInstance()->Get_Font("Bold"));
			bool opened = ImGui::TreeNode(label.c_str());
			ImGui::PopFont();

			if (opened)
			{
				for (const auto& item : pLayer->Get_Object())
				{
					ImGui::Dummy(ImVec2(0, 2));

					// ���� ���õ� ������Ʈ���� Ȯ��
					bool is_selected = (pTarget == item);

					// Ŭ�� �� Ÿ�� ����
					if (ImGui::Selectable(item->Get_InstanceName().c_str(), is_selected)) {
						pTarget = item;
						CPickingMgr::GetInstance()->Set_PickedObj(pTarget);
					}
				}
				ImGui::TreePop();
			}
		}

	}
		ImGui::End();
}


void CTestScene::Create_Object()
{
	// ȭ�� �ƹ� �������� ��Ŭ�� �� �˾� ����
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !ImGui::IsAnyItemHovered())
	{
		ImGui::OpenPopup("GlobalContextMenu");
	}

	// �˾� ��ü
	if (ImGui::BeginPopup("GlobalContextMenu"))
	{
		if (ImGui::MenuItem("Create Cube")) {
			CTestCube* instance = CTestCube::Create();
			Add_Object(LAYER_ID::L_DEFAULT, instance);
		}
		if (ImGui::MenuItem("Create Sphere")) { /* ó�� */ }
		if (ImGui::MenuItem("Create Camera")) { /* ó�� */ }
		if (ImGui::MenuItem("Create Light")) { /* ó�� */ }
		ImGui::EndPopup();
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
