#pragma once
#include "CComponent.h"
BEGIN(Engine)

class CMesh;
class CMaterial;

class ENGINE_DLL CModel :
    public CComponent
{
private:
    explicit CModel();
    virtual ~CModel();
public:
    static CModel* Create(const string& meshType);

public:
    HRESULT Ready_Component() override;
    virtual void Update_Component(_float& dt);
    virtual void LateUpdate_Component(_float& dt);
    void Render(LPDIRECT3DDEVICE9 pDevice);
    CComponent* Clone() const override;

public:
    HRESULT Set_Model(const string& meshType);
    CMesh* Get_Mesh();

private:
    CMesh* m_pMesh;
    CMaterial* m_pMaterial;

private:
    void Free() override;
};

END