#pragma once
#include "CBase.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CPrefabMgr :
    public CBase
{
    DECLARE_SINGLETON(CPrefabMgr);
public: 
    explicit CPrefabMgr();
    virtual ~CPrefabMgr();

public:
    HRESULT Ready_Prefabs();
    void Load_AllPrefabs(const string& path);
    CGameObject* Instantiate(const string& prefabName);
    const vector<string>& Get_PrefabNames() { return m_prefabList; }

private:
    unordered_map<string, json> m_prefabMap;
    vector<string> m_prefabList;
    unordered_map<string, int> m_instanceCounter;

private:
    void Free() override;
};

END