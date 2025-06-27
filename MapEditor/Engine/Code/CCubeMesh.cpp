#include "Engine_Define.h"
#include "CCubeMesh.h"
#include "CGraphicDev.h"

CCubeMesh::CCubeMesh()
	:m_pDevice(nullptr),m_pVB(nullptr),m_pIB(nullptr)
{}

CCubeMesh::~CCubeMesh()
{}

CCubeMesh* CCubeMesh::Create()
{
	CCubeMesh* instance = new CCubeMesh;

	if (FAILED(instance->Ready_Mesh())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CCubeMesh::Ready_Mesh()
{
	m_pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

	if (!m_pDevice)
		return E_FAIL;

	//HRESULT hrVB = m_pDevice->CreateVertexBuffer();

	return S_OK;
}

DWORD CCubeMesh::GetFVF() const
{
	return 0;
}

void CCubeMesh::Free()
{
}
