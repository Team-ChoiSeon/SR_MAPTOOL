#include "Engine_Define.h"
#include "CPrefabMgr.h"
#include "CGameObject.h"
#include "CSceneMgr.h"

IMPLEMENT_SINGLETON(CPrefabMgr)

CPrefabMgr::CPrefabMgr()
{
}

CPrefabMgr::~CPrefabMgr()
{
    Free();
}

HRESULT CPrefabMgr::Ready_Prefabs()
{
    Load_AllPrefabs("../../Data/ObjectData");
	return S_OK;
}

void CPrefabMgr::Load_AllPrefabs(const string& path)
{
    namespace fs = std::filesystem;

    m_prefabMap.clear(); // 기존 프리팹 제거

    try {
        for (const auto& entry : fs::directory_iterator(path))
        {
            if (!entry.is_regular_file()) continue;

            const fs::path& filePath = entry.path();
            if (filePath.extension() != ".json") continue;

            std::ifstream inFile(filePath);
            if (!inFile.is_open()) continue;

            json jPrefab;
            inFile >> jPrefab;

            string prefabName = filePath.stem().string(); // ex: "CUBE"

            m_prefabMap[prefabName] = jPrefab;
            m_prefabList.push_back(prefabName);
        }
    }
    catch (const std::exception& e) {
        MessageBoxW(0, L"프리팹 로딩 에러", L"Prefab", MB_OK);
    }
}


CGameObject* CPrefabMgr::Instantiate(const string& prefabName)
{
    auto iter = m_prefabMap.find(prefabName);
    if (iter == m_prefabMap.end()) {
        MessageBoxW(0, L"프리팹 탐색 에러", L"Prefab", MB_OK);
        return nullptr;
    }

    const json& jPrefab = iter->second;

    // GameObject
    CGameObject* pObj = CGameObject::Create(jPrefab);
    if (!pObj) {
        MessageBoxW(0, L"프리팹 생성 에러", L"Prefab", MB_OK);
        return nullptr;
    }
    if (m_instanceCounter.find(prefabName) == m_instanceCounter.end()) {
        m_instanceCounter[prefabName] = 0;
    }
    string instanceName;
    do {
        m_instanceCounter[prefabName] += 1;
        instanceName = prefabName + "_" + std::to_string(m_instanceCounter[prefabName]);
    } while (CSceneMgr::GetInstance()->Is_ObjectNameExist(instanceName));

    pObj->Set_InstanceName(instanceName);

    return pObj;
}




void CPrefabMgr::Free()
{

}
