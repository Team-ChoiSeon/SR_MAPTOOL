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

    // �� / �޽�
    CMesh* LoadMeshFromOBJ(const string& objPath);

    // �ؽ�ó
    CTexture* LoadTexture(const string& texturePath);

    // ��Ƽ����
    CMaterial* LoadMaterialFromMTL(const string& mtlPath);

    // �̸� ��� ��ȸ
    CMesh* GetMesh(const std::string& key);
    CTexture* GetTexture(const std::string& key);
    CMaterial* GetMaterial(const std::string& key);

    //�� ��ȯ
    const unordered_map<string, CMesh*>& Get_MeshMap() { return m_meshMap; }
    const unordered_map<string, CTexture*>& Get_TextureMap() { return m_textureMap; }
    const unordered_map<string, CMaterial*>& Get_MaterailMap() { return m_materialMap; }
    //�� ��ȯ
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