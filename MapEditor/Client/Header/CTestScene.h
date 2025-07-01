#pragma once
#include "CScene.h"

namespace Engine{
    class CGameObject;
}
class  CTestScene :
    public Engine::CScene
{
private:
    explicit CTestScene();
    virtual ~CTestScene();
public: 
    static CTestScene* Create();

public:
    HRESULT Ready_Scene() override;
    void Update_Scene(_float& dt) override;
    void LateUpdate_Scene(_float& dt) override;

    HRESULT Enter_Scene() override;
    HRESULT Exit_Scene() override;

private:
    void Edit_Object(CGameObject* obj);
    void Show_ObjectList();
    void Create_Object();
private:
    CGameObject* pTarget;
private:
    void Free() override;

};

