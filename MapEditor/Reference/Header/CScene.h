#pragma once
#include "CBase.h"

BEGIN(Engine)

class IPanel;
class CLayer;
class CGameObject;

class ENGINE_DLL CScene :
    public CBase
{
protected:
    explicit CScene();
    virtual ~CScene();

public:
    static CScene* Create();
    HRESULT Ready_Scene();

public:
    void Update_Scene(_float& dt) ;
    void LateUpdate_Scene(_float& dt) ;
    void Render_Panel() ;

public:
    const string& Get_Name() { return m_Name; }
    void Set_Name(const string& name) { m_Name = name; };
    CLayer* Get_Layer(string id);
    const vector<string>& Get_LayerNames() const { return m_LayerNames; }
    void Serialize(json& jScene) const;
    void Deserialize(const json& jScene);

protected:
    void Init_Layer();
    void Add_Layer(const string& layerName);
    void Free_Layer(string layer);
    void Free_AllLayer();
    HRESULT Add_Object(string layer,CGameObject* object);

protected:
    string m_Name;
    unordered_map<string, CLayer*> m_mapLayer;
    vector<string> m_LayerNames;

private:
    virtual void Free();

};

END
