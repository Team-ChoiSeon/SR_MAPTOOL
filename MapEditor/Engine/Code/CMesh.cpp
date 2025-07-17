#include "Engine_Define.h"
#include "CMesh.h"
#include "CResourceMgr.h"

//const _ulong	FVF_TILE = D3DFVF_XYZ | D3DFVF_NORMAL |D3DFVF_TEX1 ; 
//FVF�� ����Ʈ �������� ���� ���������� ������, ��������� Direct3D�� ����Ʈ �������� *�ڵ����� ����� �� �ֵ��� ��Ʈ�� �����ϴ� ����

D3DVERTEXELEMENT9 FVF_TILE[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },   // pos
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },   // normal
	{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },   // uv
	{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },   // tangent
	{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },   // binormal
	D3DDECL_END()
};

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


HRESULT CMesh::LoadOBJ(LPDIRECT3DDEVICE9 pDevice, string& path)
{

	string BasePath = "../../Resource/Obj/";
	string filePath = BasePath + path;

	//���� ����
	ifstream in;
	in.open(filePath);

	if (!in.is_open()) {
		in.clear();
		in.open("../../Resource/Obj/Default_A.obj");
		path = "Default_A.obj";
		if (!in.is_open()) {
			MessageBoxW(0, L"Mesh Load Err", L"Err", MB_OK);
			return E_FAIL;
		}
		else {
			if (CResourceMgr::GetInstance()->LoadMeshFromOBJ(path))
				return E_FAIL;
		}
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
			uv.y = 1.0f - uv.y; // Flip V (Blender ����)
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

			// �ﰢ�� ���� (fan ���)
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
					// ������Ʈ�� Transform���� ������ ��������
					//D3DXVECTOR3 scale = m_pOwner.->Get_Scale();

					// UV �������� ����ؼ� ����
					//D3DXVECTOR2 uvScale = { scale.x, scale.y };
					vertex.vTexUV.x = texcoords[vti].x;
					vertex.vTexUV.y = texcoords[vti].y;

					vertex.vPosition = positions[vi];
					vertex.vNormal = normals[vni];

					vertices.push_back(vertex);
					indices.push_back(static_cast<DWORD>(vertices.size() - 1));
				}
			}
		}
	}
	// ���� ���� ����
	
	//m_dwFVF = FVF_TILE;
	GenerateTangentSpace(vertices, indices);
	pDevice->CreateVertexDeclaration(FVF_TILE, &g_pDecl);
	m_iVertexStride = sizeof(VTXTILE);
	m_iPrimitiveCount = indices.size() / 3;
	m_iVtxCount = vertices.size();

	if (FAILED(pDevice->CreateVertexBuffer(
		vertices.size() * m_iVertexStride,
		0, 0, D3DPOOL_MANAGED, &m_pVB, 0))) {
		return E_FAIL;
	}

	void* pVBData = nullptr;
	m_pVB->Lock(0, 0, &pVBData, 0);
	memcpy(pVBData, vertices.data(), vertices.size() * m_iVertexStride);
	m_pVB->Unlock();

	// �ε��� ���� ����
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
	//���� ������ ���� ����ϴ� ��

	pDevice->SetVertexDeclaration(g_pDecl);  // ���� ���� ����
	//���� dx ���� �Ⱦ�
	//pDevice->SetFVF(m_fvf);
	pDevice->SetStreamSource(0, m_pVB, 0, m_iVertexStride);
	pDevice->SetIndices(m_pIB);
	// ������
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iVtxCount, 0, 12);
}

HRESULT CMesh::Ready_Mesh()
{
	return S_OK;
}

void CMesh::GenerateTangentSpace(vector<VTXTILE>& vertices, const vector<DWORD>& indices)
{
	// �ʱ�ȭ
	for (auto& v : vertices) {
		v.tangent = D3DXVECTOR3(0, 0, 0);
		v.binormal = D3DXVECTOR3(0, 0, 0);
	}

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		VTXTILE& v0 = vertices[indices[i + 0]];
		VTXTILE& v1 = vertices[indices[i + 1]];
		VTXTILE& v2 = vertices[indices[i + 2]];

		//3d ������ ��ġ ��ȭ��
		D3DXVECTOR3 edge1 = v1.vPosition - v0.vPosition; 
		D3DXVECTOR3 edge2 = v2.vPosition - v0.vPosition;

		//���� �����Ǵ� �ؽ�ó ��ȭ��
		D3DXVECTOR2 deltaUV1 = v1.vTexUV - v0.vTexUV;
		D3DXVECTOR2 deltaUV2 = v2.vTexUV - v0.vTexUV;

		float f = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
		if (fabs(f) < 1e-6f) f = 1.0f; // divide by zero ����
		float invDet = 1.0f / f;

		D3DXVECTOR3 tangent = (edge1 * deltaUV2.y - edge2 * deltaUV1.y) * invDet;
		D3DXVECTOR3 binormal = (edge2 * deltaUV1.x - edge1 * deltaUV2.x) * invDet;

		// ���� (������ �����̶� ��� �� ����)
		v0.tangent += tangent;
		v1.tangent += tangent;
		v2.tangent += tangent;

		v0.binormal += binormal;
		v1.binormal += binormal;
		v2.binormal += binormal;
	}

	// ����ȭ ������ �� ��� �� �Ͱ� �������� ȿ��(������ �޴� ������ ����ױ�)
	for (auto& v : vertices) {
		D3DXVec3Normalize(&v.tangent, &v.tangent);
		D3DXVec3Normalize(&v.binormal, &v.binormal);
	}
}


void CMesh::Free()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}

