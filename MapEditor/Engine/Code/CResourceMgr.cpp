#include "Engine_Define.h"
#include "CResourceMgr.h"
#include "IMesh.h"
#include "CCubeMesh.h"

IMPLEMENT_SINGLETON(CResourceMgr)

CResourceMgr::CResourceMgr()
{
}

CResourceMgr::~CResourceMgr()
{
	Free();
}

HRESULT CResourceMgr::Ready_Resource()
{
	m_umMeshContainer.insert({"CUBE", CCubeMesh::Create()});
	return S_OK;
}

IMesh* CResourceMgr::Get_Mesh(const string& key)
{
	auto iter = m_umMeshContainer.find(key);

	if (iter == m_umMeshContainer.end())
		return nullptr;

	return iter->second;
}

void CResourceMgr::Free()
{
	for (auto& data : m_umMeshContainer) {
		Safe_Release(data.second);
	}

	m_umMeshContainer.clear();
}
