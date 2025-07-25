#pragma once
#include "CComponent.h"
BEGIN(Engine)

class ENGINE_DLL  CTransform :
    public CComponent
{
private:
    explicit CTransform();
    virtual ~CTransform();

public:
    static CTransform* Create();

public:
    HRESULT Ready_Component() override;
    void Update_Component(_float& dt) override;
    void LateUpdate_Component(_float& dt) override;

    CComponent* Clone() const override;

public:
    void Render_Panel(ImVec2 size) override;
    void Serialize(json& outJson) const override;
    void Deserialize(const json& inJson) override;

    void Set_Parent(CTransform* transform);
    void Set_Pos(_vec3 pos);
    void Set_Scale(_vec3 scale);
    void Set_Rotate(_vec3 rot);
    void Set_Pivot(_vec3 pivot);
    void Set_Orbit(_vec3 orbit);
    void Set_Look(_vec3 look);

    _vec3 Get_Pos() { return m_vPos; }
    _vec3 Get_Scale() { return m_vScale; }
    _vec3 Get_Rotate() { return m_vRotate; }
    _vec3 Get_Pivot() { return m_vPivot; }
    _vec3 Get_Orbit() { return m_vOrbit; }
    _vec3 Get_Look() { return m_vLook; }
    _vec3 Get_Right(){return m_vRight; }
    _vec3 Get_Up() { return m_vUp; }

    string Get_ComponentName() const override;
    CGameObject* Get_Parent() { if (m_pParent) return m_pParent->m_pOwner;  return nullptr; }

    const _matrix& Get_WorldMatrix() const { return m_WorldMat; }
    const _matrix& Get_WorldPosMatrix() const { return m_WorldPosMat; }

    void Add_Pos(_vec3 pos);
    void Add_Scale(_vec3 scale);
    void Add_Rotate(_vec3 rot);
    void Add_Pivot(_vec3 pivot);
    void Add_Orbit(_vec3 orbit);
    void Add_Axis(_vec3 axis);
    void Rotate_Axis(const _vec3& axis, float angle);
    void Set_LookDirection(const _vec3& lookDir);


private:
    void Free() override;

private:
    _vec3 m_vPos;
    _vec3 m_vScale;
    _vec3 m_vRotate;
    _vec3 m_vAxisRotate;
    _vec3 m_vPivot;
    _vec3 m_vOrbit;

    _vec3 m_vLook;
    _vec3 m_vRight;
    _vec3 m_vUp;

    _matrix m_WorldMat;
    _matrix m_WorldPosMat;
    CTransform* m_pParent;

};

END

