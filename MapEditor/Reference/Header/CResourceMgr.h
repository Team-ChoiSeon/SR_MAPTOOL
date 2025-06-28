#pragma once
#include "CBase.h"
BEGIN(Engine)
class IMesh;

class ENGINE_DLL CResourceMgr :
    public CBase
{
    DECLARE_SINGLETON(CResourceMgr)

private:
    explicit CResourceMgr();
    virtual ~CResourceMgr();

public:
    HRESULT Ready_Resource();

public:
    IMesh* Get_Mesh(const string& key);

private:
    unordered_map<string, IMesh*> m_umMeshContainer;

private:
    void Free();
};

END