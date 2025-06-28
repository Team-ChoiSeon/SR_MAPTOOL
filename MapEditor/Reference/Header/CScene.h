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
    virtual void Render_Panel();
public:
    virtual HRESULT Enter_Scene()PURE;
    virtual HRESULT Exit_Scene()PURE;

public:
    const string& Get_Name() { return m_Name; }
    void Set_Name(const string& name) { m_Name = name; };

protected:
   void Update_Panel(_float& dt);
   void LateUpdate_Panel(_float& dt);
   void FreeUI();
    void Create_Layer(const string& key);
    void Free_Layer(const string& key);
    void Free_AllLayer();

protected:
    string m_Name;
    unordered_map<type_index, IPanel*> m_UIContainer;//gui panel
    unordered_map<string, CLayer*> m_mapLayer;

private:
    virtual void Free() PURE;

public:
    template<typename T, typename ...Args>
    T* Add_Panel(Args && ...args);
    template<typename T>
    T* Find_Panel();
};

template<typename T, typename ...Args>
inline T* CScene::Add_Panel( Args&&... args)
{
    if (T* exist = Find_Panel<T>())
        return exist;

    T* instance = T::Create(std::forward<Args>(args)...);
    if (!instance) return nullptr;

    instance->m_pOwner = this;
    m_UIContainer.insert({ type_index(typeid(T)),instance });

    return instance;
}

template<typename T>
inline T* CScene::Find_Panel()
{
    auto iter = m_UIContainer.find(type_index(typeid(T)));

    if (iter != m_UIContainer.end())
        return dynamic_cast<T*>(iter->second);

    return nullptr;
}

END
