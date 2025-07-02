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
    vector<CGameObject*>&  Get_Object() { return m_ObjectList; };

public:
    HRESULT Add_Object(CGameObject* object);
    CGameObject* Find_Object(const string& tag);
    void Remove_Object(const string& tag);
    void Pop_Object(const string& tag);
    _bool Has_Object(CGameObject* obj);
private:
    HRESULT Ready_Layer();

private:
    vector<CGameObject*> m_ObjectList;

private:
    void Free();
};

END