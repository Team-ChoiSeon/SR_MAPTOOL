#pragma once
#include "CBase.h"
BEGIN(Engine)
class IMesh;

class CTexture;
class CMesh;
class CMaterial;

class ENGINE_DLL CResourceMgr :
    public CBase
{
    DECLARE_SINGLETON(CResourceMgr)

private:
    explicit CResourceMgr();
    virtual ~CResourceMgr();

public:
    HRESULT Ready_Resource();
    // 모델 / 메쉬
    CMesh* LoadMeshFromOBJ(const std::string& objPath);

    // 텍스처
    CTexture* LoadTexture(const std::string& texturePath);

    // 머티리얼
    CMaterial* LoadMaterialFromMTL(const std::string& mtlPath);

    // 이름 기반 조회
    CMesh* GetMesh(const std::string& key);
    CTexture* GetTexture(const std::string& key);
    CMaterial* GetMaterial(const std::string& key);

private:
    LPDIRECT3DDEVICE9 m_pDevice;

    std::unordered_map<std::string, CMesh*> m_meshMap;
    std::unordered_map<std::string, CTexture*> m_textureMap;
    std::unordered_map<std::string, CMaterial*> m_materialMap;
private:
    void Free();
};

END