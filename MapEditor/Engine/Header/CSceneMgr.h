#pragma once
#include "CBase.h"

BEGIN(Engine)
class CScene;
class CGameObject;

class ENGINE_DLL CSceneMgr :
    public CBase
{
    DECLARE_SINGLETON(CSceneMgr)

private:
    explicit CSceneMgr();
    virtual ~CSceneMgr();

public:
    HRESULT Ready_SceneMgr();
    void Update_Scene(_float& dt);
    void LateUpdate_Scene(_float& dt);
    void Render_Scene();

public:
    CScene* Get_NowScene() { return m_CurScene; };
    void Add_Scene(string sceneTag, CScene* scene);
    void Change_Scene(string sceneTag);
    const unordered_map<string, CScene*>& Get_SceneArr()const { return m_SceneContainer; }
    const vector<string>& SceneName() { return m_SceneList; };

private:
    void Load_SceneData(const string& scenePath);
    void Render_SceneSelector();
    void Save_LoadPanel();

public: 
    void SerializeScene(const wstring& path);
    void DeSerializeScene(const wstring& path);
    void Create_Object();
    bool Is_ObjectNameExist(const string& name) const;
    CGameObject* Get_SelectedObject() { return selectObj; };
    void Set_SelectedObject(CGameObject* obj) { selectObj = obj; };
private:
    unordered_map<string, CScene*> m_SceneContainer;
    vector<string> m_SceneList;                      
    int m_SceneIndex = 0;
    CScene* m_CurScene;
    CGameObject* selectObj = nullptr;
private:
    void Free();

};
END
