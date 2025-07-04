#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CTransform;

class ENGINE_DLL CCamera :
    public CComponent
{
private:
    explicit CCamera();
    virtual ~CCamera();

public:
    static CCamera* Create();
public:
    HRESULT Ready_Component() override;
    virtual void Update_Component(_float& dt);
    virtual void LateUpdate_Component(_float& dt);
    virtual CComponent* Clone() const;

public:
    void Render_Panel(ImVec2 size) override;
    void Serialize(json& outJson) const override;
    void Deserialize(const json& inJson) override;


    void Set_View(_vec3 _vEye, _vec3 _vUP, _vec3 _vDir);
    void Set_Proj(_float _FOV, _float _near, _float _far);
    void Set_YawPitchRoll(_vec3 axis);

    _matrix& Get_ViewMatrix() { return m_matView; }
    _matrix& Get_ProjMatrix() { return m_matProj; }

    void Add_Yaw(_float angle);
    void Add_Pitch(_float angle);
    _float Get_FOV() { return m_fFOV; }
    _float Get_Aspect() { return m_fAspect; }
    _float Get_Near() { return m_fNear; }
    _float Get_Far() { return m_fFar; }

    _float Get_Pitch() { return m_fPitch; }
    _float Get_Yaw() { return m_fYaw; }
    _float Get_Roll() { return m_fRoll; }
    

    _vec3 Get_Dir() { return m_vLookDir; };
    _vec3 Get_Eye() { return m_vEye; };
    _vec3 Get_Up() { return m_vUp; };
    _vec3 Get_Right() { 
        _vec3 Right;
        D3DXVec3Cross(&Right, &m_vUp, &m_vLookDir); 
        return Right;
    };

private:
    void UpdateDirFromAngles();
private:
    _float m_fFOV;
    _float m_fAspect;
    _float m_fNear;
    _float m_fFar;

    _float m_fPitch;
    _float m_fYaw;
    _float m_fRoll;

    _vec3 m_vUp;
    _vec3 m_vEye;
    _vec3 m_vLookDir;

    _matrix m_matView;
    _matrix m_matProj;
    CTransform* m_pTransform;

private:
    void Free();

};

END