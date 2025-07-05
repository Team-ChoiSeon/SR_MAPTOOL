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

private:
    CGameObject* pTarget;
    string curBuffer;
    string prevBuffer;
private:
    void Free() override;

};

