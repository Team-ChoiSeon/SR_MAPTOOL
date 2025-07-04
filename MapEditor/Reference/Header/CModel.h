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
    static CModel* Create();

public:
    HRESULT Ready_Component() override;
    virtual void Update_Component(_float& dt);
    virtual void LateUpdate_Component(_float& dt);
    void Render(LPDIRECT3DDEVICE9 pDevice);
    RENDER_PASS Get_RenderPass();
    CComponent* Clone() const override;

public:
    HRESULT Set_Model(const string& meshType, const string& matType );
    void Render_Panel(ImVec2 size) override;
    void Serialize(json& outJson) const override;
    void Deserialize(const json& inJson) override;

    CMesh* Get_Mesh();
    CMaterial* Get_Material();
    AABB Get_AABB();
    string Get_ComponentName() const override;

private:
    CMesh* m_pMesh;
    CMaterial* m_pMaterial;
    int m_iMeshIndex;
    int m_iMaterialIndex;
    int m_iShaderIndex;

private:
    void Free() override;

   
};

END