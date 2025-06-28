#pragma once
#include "CGameObject.h"

class CTestCube :
    public Engine::CGameObject
{
private:
    explicit CTestCube();
    virtual ~CTestCube();
public:
    static CTestCube* Create();
public:
    HRESULT Ready_GameObject() override;
    void Update_GameObject(_float& dt) override;
    void LateUpdate_GameObject(_float& dt) override;

private:
    void Free() override;
};

