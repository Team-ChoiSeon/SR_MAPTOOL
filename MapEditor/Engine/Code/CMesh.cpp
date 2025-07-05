#include "Engine_Define.h"
#include "CMesh.h"

CMesh::CMesh()
{
}

CMesh::~CMesh()
{
}

CMesh* CMesh::Create()
{
	CMesh* instance = new CMesh;

	if (FAILED(instance->Ready_Mesh())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}


HRESULT CMesh::LoadOBJ(LPDIRECT3DDEVICE9 pDevice, const std::string& path)
{
	//파일 열기
	ifstream in(path);
	if (!in.is_open()) {
		MSG_BOX(L"OBJ 로딩 파일 오류가 생겼습니다");
		return E_FAIL;
	}

	vector<D3DXVECTOR3> positions;
	vector<D3DXVECTOR3> normals;
	vector<D3DXVECTOR2> texcoords;
	vector<VTXTILE>    vertices;
	vector<DWORD>        indices;

	std::string line;
	while (std::getline(in, line))
	{
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;

		if (prefix == "v") {
			D3DXVECTOR3 pos;
			iss >> pos.x >> pos.y >> pos.z;
			positions.push_back(pos);
		}
		else if (prefix == "vt") {
			D3DXVECTOR2 uv;
			iss >> uv.x >> uv.y;
			uv.y = 1.0f - uv.y; // Flip V (Blender 기준)
			texcoords.push_back(uv);
		}
		else if (prefix =="vn") {
			D3DXVECTOR3 nv;
			iss >> nv.x >> nv.y >> nv.z;
			normals.push_back(nv);
		}
		else if (prefix == "f") {
			std::vector<std::string> tokens;
			std::string token;
			while (iss >> token)
				tokens.push_back(token);

			// 삼각형 분할 (fan 방식)
			for (size_t i = 1; i + 1 < tokens.size(); ++i) {
				std::string vStr[3] = { tokens[0], tokens[i], tokens[i + 1] };

				for (int k = 0; k < 3; ++k) {
					std::istringstream vstream(vStr[k]);
					std::string viStr, vtiStr, vniStr;
					std::getline(vstream, viStr, '/');
					std::getline(vstream, vtiStr, '/');
					std::getline(vstream, vniStr, '/');

					int vi = std::stoi(viStr) - 1;
					int vti = std::stoi(vtiStr) - 1;
					int vni = std::stoi(vniStr) - 1;

					VTXTILE vertex;
					// 오브젝트의 Transform에서 스케일 가져오기
					//D3DXVECTOR3 scale = m_pOwner.->Get_Scale();

					// UV 스케일을 비례해서 설정
					//D3DXVECTOR2 uvScale = { scale.x, scale.y };
					D3DXVECTOR2 uvScale = { 1.f, 1.f };
					vertex.vTexUV.x = texcoords[vti].x * uvScale.x;
					vertex.vTexUV.y = texcoords[vti].y * uvScale.y;

					vertex.vPosition = positions[vi];
					vertex.vNormal = normals[vni];

					vertices.push_back(vertex);
					indices.push_back(static_cast<DWORD>(vertices.size() - 1));
				}
			}
		}
	}
	m_tAABB.vMin = { FLT_MAX, FLT_MAX, FLT_MAX };
	m_tAABB.vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (const auto& pos : positions) //로컬 좌표를 순회
	{
		if (pos.x < m_tAABB.vMin.x) m_tAABB.vMin.x = pos.x;
		if (pos.y < m_tAABB.vMin.y) m_tAABB.vMin.y = pos.y;
		if (pos.z < m_tAABB.vMin.z) m_tAABB.vMin.z = pos.z;

		if (pos.x > m_tAABB.vMax.x) m_tAABB.vMax.x = pos.x;
		if (pos.y > m_tAABB.vMax.y) m_tAABB.vMax.y = pos.y;
		if (pos.z > m_tAABB.vMax.z) m_tAABB.vMax.z = pos.z;
	}

	// 정점 버퍼 생성
	
	m_dwFVF = FVF_TILE;
	m_iVertexStride = sizeof(VTXTILE);
	m_iPrimitiveCount = indices.size() / 3;
	m_iVtxCount = vertices.size();

	if (FAILED(pDevice->CreateVertexBuffer(
		vertices.size() * m_iVertexStride,
		0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, 0))) {
		return E_FAIL;
	}

	void* pVBData = nullptr;
	m_pVB->Lock(0, 0, &pVBData, 0);
	memcpy(pVBData, vertices.data(), vertices.size() * m_iVertexStride);
	m_pVB->Unlock();

	// 인덱스 버퍼 생성
	if (FAILED(pDevice->CreateIndexBuffer(
		indices.size() * sizeof(DWORD),
		0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIB, 0))) {
		return E_FAIL;
	}

	void* pIBData = nullptr;
	m_pIB->Lock(0, 0, &pIBData, 0);
	memcpy(pIBData, indices.data(), indices.size() * sizeof(DWORD));
	m_pIB->Unlock();

	return S_OK;
}



void CMesh::Render(LPDIRECT3DDEVICE9 pDevice)
{

	pDevice->SetFVF(m_dwFVF);
	pDevice->SetStreamSource(0, m_pVB, 0, m_iVertexStride);
	pDevice->SetIndices(m_pIB);
	// 렌더링
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iVtxCount, 0, 12);
}

HRESULT CMesh::Ready_Mesh()
{
	return S_OK;
}

void CMesh::Free()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}

