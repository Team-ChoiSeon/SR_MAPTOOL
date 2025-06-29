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

public:
    void Add_Object(const string& tag, CGameObject* object);
    vector<CGameObject*>&  Get_Object() { return m_ObjectList; };
    CGameObject* Find_Object(const string& tag);
    void Remove_Object(const string& tag);

private:
    HRESULT Ready_Layer();

private:
    vector<CGameObject*> m_ObjectList;

private:
    void Free();
};

END