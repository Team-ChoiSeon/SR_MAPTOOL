#pragma once
#include "CGameObject.h"

namespace Engine {
    class CTransform;
    class CCamera;
}

class CCameraActor :
    public Engine::CGameObject
{
public :
    enum class Camera_Mode {Picking, WorldMove, Rotate };

private:
    explicit CCameraActor();
    virtual ~CCameraActor();
public:
    static CCameraActor* Create();
public:
    HRESULT Ready_GameObject() override;
    void Update_GameObject(_float& dt) override;
    void LateUpdate_GameObject(_float& dt) override;

private:
    void Mode_Check(_float& dt);
    void Key_Check(_float& dt);
    void CamModePanel();
private:
    CTransform* m_pTransform;
    CCamera* m_pCamera;
    Camera_Mode m_eMode;
    POINT m_tAnchor;

    float m_fXmove;
    float m_fYmove;
private:
    void Free() override;
};

