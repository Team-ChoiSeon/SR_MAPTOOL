#include "Engine_Define.h"
#include "CCubeMesh.h"
#include "CGraphicDev.h"

CCubeMesh::CCubeMesh()
	:m_pDevice(nullptr),m_pVB(nullptr),m_pIB(nullptr), 
    m_FVF(FVF_TILE), numIndices(36), numVertices(8)
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
    
    m_pDevice->AddRef();

    // ���� ���� ����
    HRESULT hr = m_pDevice->CreateVertexBuffer(sizeof(VTXTILE) * numVertices,
        0, m_FVF, D3DPOOL_MANAGED, &m_pVB, 0);

    if (FAILED(hr)) {
        return hr;
    }

    // �ε��� ���� ����
    hr = m_pDevice->CreateIndexBuffer(sizeof(DWORD) * numIndices,
        0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIB, 0);

    if (FAILED(hr)) {
        return hr;
    }


    float s = 1.f;

    VTXTILE cube[] = {
        // �޸� (-Z)
        { {-s,+s,-s}, { 0,  0, -1 }, {0, 1} },      // 0
        { {+s,+s,-s}, { 0,  0, -1 }, {0, 0} },      // 1
        { {+s,-s,-s}, { 0,  0, -1 }, {1, 0} },      // 2
        { {-s,-s,-s}, { 0,  0, -1 }, {1, 1} },      // 3

        // �ո� (+Z)
        { {-s,+s,+s}, { 0,  0,  1 }, {0, 0} },  // 4
        { {+s,+s,+s}, { 0,  0,  1 }, {0, 0} }, // 5
        { {+s,-s,+s}, { 0,  0,  1 }, {0, 0} },  // 6
        { {-s,-s,+s}, { 0,  0,  1 }, {0, 0} },  // 7
    };

    m_tAABB.vMin = { -s,-s,-s } ;
    m_tAABB.vMax = { +s,+s,+s };

    DWORD indices[] =
    {
        3,0,1,   3,1,2,     //�ո�
        2,1,5,   2,5,6,     //�����ʸ�
        6,5,4,  6,4,7,  // �޸�
        0,4,5,  0,5,1,   // ����
        7,4,0,  7,0,3,  //�޸�
        7,3,2,  7,2,6   //�Ʒ���
    };

    // ���� ����
    void* pVertices = nullptr;
    m_pVB->Lock(0, 0, &pVertices, m_FVF);
    memcpy(pVertices, cube, sizeof(cube));
    m_pVB->Unlock();

    // �ε��� ����
    void* pIndices = nullptr;
    m_pIB->Lock(0, 0, &pIndices, D3DFMT_INDEX32);
    memcpy(pIndices, indices, sizeof(indices));
    m_pIB->Unlock();

	return S_OK;
}

void CCubeMesh::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pIB);
    Safe_Release(m_pVB);
}
