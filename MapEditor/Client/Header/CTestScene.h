#pragma once
#include "CScene.h"

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

private:
    void Free() override;
};

