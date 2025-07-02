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
    // �� / �޽�
    CMesh* LoadMeshFromOBJ(const std::string& objPath);

    // �ؽ�ó
    CTexture* LoadTexture(const std::string& texturePath);

    // ��Ƽ����
    CMaterial* LoadMaterialFromMTL(const std::string& mtlPath);

    // �̸� ��� ��ȸ
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