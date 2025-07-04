#pragma once
#include "CBase.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CLayer :
    public CBase
{
private:
    explicit CLayer();
    virtual ~CLayer();
public:
    static CLayer* Create();
    void Update_Layer(_float& dt);
    void LateUpdate_Layer(_float& dt);
    void Render_Panel();

public:
    HRESULT Add_Object(CGameObject* object);
    void Pop_Object(const string& tag);
    void Remove_Object(const string& tag);
    vector<CGameObject*>&  Get_Object() { return m_ObjectList; };

public:
    void Serialize(json& jLayer) const;
    void Deserialize(const json& injLayer);

private:
    HRESULT Ready_Layer();

private:
    string m_LayerName;
    vector<CGameObject*> m_ObjectList;

private:
    void Free();
};

END