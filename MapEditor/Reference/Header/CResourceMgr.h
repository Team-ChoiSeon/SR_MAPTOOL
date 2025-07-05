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
    void AutoLoad_OBJMeshes();
    void AutoLoad_MaterialFromMTL();
    int Get_MeshID(const string& name);
    int Get_MaterialID(const string& name);

    // 모델 / 메쉬
    CMesh* LoadMeshFromOBJ(const string& objPath);

    // 텍스처
    CTexture* LoadTexture(const string& texturePath);

    // 머티리얼
    CMaterial* LoadMaterialFromMTL(const string& mtlPath);

    // 이름 기반 조회
    CMesh* GetMesh(const std::string& key);
    CTexture* GetTexture(const std::string& key);
    CMaterial* GetMaterial(const std::string& key);

    //맵 반환
    const unordered_map<string, CMesh*>& Get_MeshMap() { return m_meshMap; }
    const unordered_map<string, CTexture*>& Get_TextureMap() { return m_textureMap; }
    const unordered_map<string, CMaterial*>& Get_MaterailMap() { return m_materialMap; }
    //맵 반환
    const vector<string>& Get_MeshName() { return m_meshName; }
    const vector<string>& Get_TextureName() { return m_textureName; }
    const vector<string>& Get_MaterialName() { return m_materialName; }

private:
    LPDIRECT3DDEVICE9 m_pDevice;

   unordered_map<string, CMesh*> m_meshMap;
   unordered_map<string, CTexture*> m_textureMap;
   unordered_map<string, CMaterial*> m_materialMap;

   vector<string> m_meshName;
   vector<string> m_textureName;
   vector<string> m_materialName;
private:
    void Free();
};

END