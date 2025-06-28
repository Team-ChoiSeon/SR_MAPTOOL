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
    void Add_Object(CGameObject* object);
    void Add_ObjectList(vector<CGameObject*> obj);
    void Remove_Object(function<bool(CGameObject*)> predicate);
    vector<CGameObject*> Pop_Objects(function<bool(CGameObject*)> predicate);

private:
    HRESULT Ready_Layer();

private:
    vector<CGameObject*> m_ObjectList;

private:
    void Free();
};

END