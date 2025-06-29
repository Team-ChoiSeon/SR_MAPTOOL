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

protected:
    void Create_Layer(const string& key);
    void Free_Layer(const string& key);
    void Free_AllLayer();

protected:
    string m_Name;
    unordered_map<string, CLayer*> m_mapLayer;

private:
    virtual void Free() PURE;

};

END
