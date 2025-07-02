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
    virtual HRESULT Ready_Scene() PURE;

public:
    virtual void Update_Scene(_float& dt) PURE;
    virtual void LateUpdate_Scene(_float& dt) PURE;
public:
    virtual HRESULT Enter_Scene()PURE;
    virtual HRESULT Exit_Scene()PURE;

public:
    const string& Get_Name() { return m_Name; }
    void Set_Name(const string& name) { m_Name = name; };
    CLayer* Get_Layer(LAYER_ID id);
    const char* Layer_ToString(LAYER_ID id);

protected:
    void Init_Layer();
    void Swap_Layer(const string& from, const string& to);
    void Free_Layer(LAYER_ID layer);
    void Free_AllLayer();
    HRESULT Add_Object(const string& tag, LAYER_ID layer,CGameObject* object);
protected:
    string m_Name;
    unordered_map<LAYER_ID, CLayer*> m_mapLayer;

private:
    virtual void Free() PURE;

};

END
