#pragma once
#include "CBase.h"

BEGIN(Engine)
class CScene;

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
    void Render_SceneSelector();
    void Save_LoadPanel();

public: 
    void SerializeScene(const wstring& path);
    void DeSerializeScene(const wstring& path);

private:
    unordered_map<string, CScene*> m_SceneContainer;
    vector<string> m_SceneList;                      
    int m_SceneIndex = 0;
    CScene* m_CurScene;

private:
    void Free();


};
END
