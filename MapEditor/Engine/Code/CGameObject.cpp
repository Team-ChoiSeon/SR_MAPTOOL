#include "Engine_Define.h"
#include "CGameObject.h"
#include "CSceneMgr.h"
#include "CFunction.h"
#include "CTransform.h"
#include "CModel.h"
#include "CCamera.h"
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

HRESULT CGameObject::Ready_GameObject(json jObj)
{
    Deserialize(jObj);
	return S_OK;
}

string CGameObject::Get_InstanceName()
{
	return this->Get_Name() + to_string(ObjCount);
}

void CGameObject::Update_GameObject(_float& dt)
{
	for (auto& pair : m_ComponentMap)
		pair.second->Update_Component(dt);
}

void CGameObject::LateUpdate_GameObject(_float& dt)
{
	for (auto& pair : m_ComponentMap)
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
        // Ŭ������ (�б� ����)
        ImGui::Text("Class: %s", ClassName.c_str());

        // �ν��Ͻ� �̸� (���� ����)
        char nameBuf[64];
        strcpy_s(nameBuf, InstanceName.c_str());
        if (ImGui::InputText("Instance Name", nameBuf, IM_ARRAYSIZE(nameBuf))) {
            InstanceName = nameBuf;
        }

        // ���̾� ��Ӵٿ�
        const vector<string>& layerList = CSceneMgr::GetInstance()->Get_NowScene()->Get_LayerNames(); // ���� string
        static int currentLayerIndex = 0;

        for (int i = 0; i < layerList.size(); ++i) {
            if (layerList[i] == LayerName) {
                currentLayerIndex = i;
                break;
            }
        }

        const char* currentPreview = layerList[currentLayerIndex].c_str();
        if (ImGui::BeginCombo("Layer", currentPreview)) {
            for (int i = 0; i < layerList.size(); ++i) {
                bool isSelected = (i == currentLayerIndex);
                if (ImGui::Selectable(layerList[i].c_str(), isSelected)) {
                    currentLayerIndex = i;
                    LayerName = layerList[i]; // �ݿ�
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();
    }

    for (auto& pair : m_ComponentMap)
        pair.second->Render_Panel(size);

    ImGui::End();
}

void CGameObject::Serialize(json& outJson) const
{
    // �⺻ ����
    outJson["class"] = ClassName;
    outJson["name"] = InstanceName;
    outJson["Layer"] = LayerName;

    // ������Ʈ �ø��������
    json jComponents;

    for (const auto& pair : m_ComponentMap)
    {
        const CComponent* comp = pair.second;
        if (!comp)
            continue;

        // ������Ʈ �̸� ����
        string compName = typeid(*comp).name();  // ��: "class CTransform"
        size_t pos = compName.find_last_of(' ');
        if (pos != std::string::npos)
            compName = compName.substr(pos + 1);      // �� "CTransform"

        json jComp;
        comp->Serialize(jComp);
        jComponents[compName] = jComp;
    }

    outJson["components"] = jComponents;
}

void CGameObject::Deserialize(const json& inJson)
{
    ClassName = inJson["class"];
    InstanceName = inJson["name"];
    const json& jComps = inJson["components"];

    TryLoadComponent<CTransform>(this, jComps, "CTransform");
    TryLoadComponent<CCamera>(this, jComps, "CTransform");
    TryLoadComponent<CModel>(this, jComps, "CModel");
}

void CGameObject::Free()
{
	for (auto& pair : m_ComponentMap)
		Safe_Release(pair.second);
}

/*\
{
    "class": "CEnemy",
        "name" : "Goblin01",
        "Layer" : "Enemy",
        "components" : {
        "CTransform": {
            "position": [0.0, 0.0, 0.0] ,
                "rotation" : [0.0, 0.0, 0.0] ,
                "scale" : [1.0, 1.0, 1.0]
        },
            "CMeshRenderer" : {
            "meshKey": "GoblinMesh",
                "materialKey" : "GoblinMaterial"
        },
            "CAnimator" : {
            "animSet": "Goblin.animset",
                "currentAnim" : "Idle"
        }
    }
}
*/