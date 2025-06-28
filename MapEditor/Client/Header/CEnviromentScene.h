#pragma once
#include "CScene.h"

class CEnviromentScene :
    public Engine::CScene
{
private:
    explicit CEnviromentScene();
    virtual ~CEnviromentScene();

public:
    static CEnviromentScene* Create();

public:
    HRESULT Ready_Scene() override;
    void Update_Scene(_float& dt) override;
    void LateUpdate_Scene(_float& dt) override;   
    HRESULT Enter_Scene() override;
    HRESULT Exit_Scene() override;
private:
    void Free() override;


};

