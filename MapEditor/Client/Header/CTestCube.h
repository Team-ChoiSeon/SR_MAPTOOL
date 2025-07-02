#pragma once
#include "CGameObject.h"

namespace Engine {
    class CTransform;
}
class CTestCube :
    public Engine::CGameObject
{
public:
    static int objCount;
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
    CTransform* m_pTransform;
private:
    void Free() override;
};

