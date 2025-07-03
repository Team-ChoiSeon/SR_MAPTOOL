#include "Engine_Define.h"
#include "CResourceMgr.h"

#include "CGraphicDev.h"
#include "CMesh.h"
#include "CMaterial.h"
#include "CTexture.h"


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
	m_pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();
	if (!m_pDevice) return E_FAIL;

	LoadTexture("dirt_diff_4k.jpg");
	LoadMaterialFromMTL("DirtObj.mtl");
	LoadMeshFromOBJ("DirtObj.obj");
	return S_OK;
}

CMesh* CResourceMgr::LoadMeshFromOBJ( const std::string& objPath)
{
	auto iter = m_meshMap.find(objPath);
	if (iter != m_meshMap.end()) {
		return iter->second;
	}

	string BasePath = "../Bin/Resource/Obj/";
	string filePath = BasePath + objPath;

	CMesh* mesh = CMesh::Create();
	if (FAILED(mesh->LoadOBJ(m_pDevice,filePath))) {
		Safe_Release(mesh);
		return nullptr;
	}
	mesh->Set_Key(objPath);
	m_meshMap[objPath] = mesh;
	return mesh;
}

CTexture* CResourceMgr::LoadTexture(const std::string& texturePath)
{
	auto it = m_textureMap.find(texturePath);
	if (it != m_textureMap.end()) 
		return it->second;


	string BasePath = "../Bin/Resource/Texture/";
	string filePath = BasePath + texturePath;

	CTexture* tex = CTexture::Create();

	if (FAILED(tex->Load(m_pDevice, filePath))) {
		Safe_Release(tex);
		return nullptr;
	}
	tex->SetKey(texturePath);
	m_textureMap[texturePath] = tex;
	return tex;
}

CMaterial* CResourceMgr::LoadMaterialFromMTL(const std::string& mtlPath)
{
	// �̹� �ε�� ��Ƽ������ �ִٸ� ���� (�ߺ� ����)
	auto iter = m_materialMap.find(mtlPath);
	if (iter != m_materialMap.end())
		return iter->second;

	// �ؽ�ó ���� ��Ʈ ��� ���� ( �ؽ�ó ���� ��� ���տ�)
	string BasePath = "../Bin/Resource/Material/";
	string filePath = BasePath + mtlPath;

	// .mtl ���� ����
	ifstream in(filePath);
	if (!in.is_open()) {
		MSG_BOX(L"��Ƽ���� �ε� ���� ������ ������ϴ�");
		return nullptr;
	}

	// .mtl ���� �Ľ� �غ�
	string line;
	string matName;
	string texturePath;

	// �� �پ� �����鼭 �Ľ�
	while (std::getline(in, line))//���پ�
	{
		istringstream iss(line);
		string token;
		iss >> token; //line ���ڿ��� ����(����) �������� ��ū ������ �и�

		// ��Ƽ���� �̸� ����
		if (token == "newmtl") {
			iss >> matName;//newmtl�� ������ �״����� ��Ƽ���� ����
		}
		// ��ǻ�� �ؽ�ó ��� ����
		else if (token == "map_Kd") {
			iss >> texturePath;//map_Kd�� ������ ������ �ؽ���

			// ��� ���ڿ����� ������ ������(/ �Ǵ� \)�� ��ġ�� ã��
			// ��: "C:/folder/texture.jpg" �� ������ '/'�� index 11
			size_t pos = texturePath.find_last_of("/\\");

			// ã�Ҵٸ� (npos�� �ƴ϶��) �� ���ϸ� ����
			// ��: substr(12) �� "texture.jpg"
			if (pos != std::string::npos)
				texturePath = texturePath.substr(pos + 1);
		}
	}

	// �ؽ�ó �ε� �� ��� ���� (�� ���� ��� ������ ���� ���� ����)
	CTexture* tex = LoadTexture(texturePath); // ��� ���� �ʿ�
	if (!tex) return nullptr;

	// ��Ƽ���� ��ü ���� �� �ؽ�ó ����
	CMaterial* mat = CMaterial::Create();
	mat->Set_Key(mtlPath);
	mat->Set_Diffuse(tex);
	// ��Ƽ���� �ʿ� ��� (�̸����� ����)
	m_materialMap[mtlPath] = mat;

	return mat;
}

CMesh* CResourceMgr::GetMesh(const std::string& key)
{
	auto iter =m_meshMap.find(key);
	if (iter != m_meshMap.end()) {
		return iter->second;
	}
	return nullptr;
}

CTexture* CResourceMgr::GetTexture(const std::string& key)
{
	auto iter = m_textureMap.find(key);
	if (iter != m_textureMap.end()) {
		return iter->second;
	}
	return nullptr;
}

CMaterial* CResourceMgr::GetMaterial(const std::string& key)
{
	auto iter = m_materialMap.find(key);
	if (iter != m_materialMap.end()) {
		return iter->second;
	}
	return nullptr;
}

void CResourceMgr::Free()
{
	for (auto& data : m_textureMap) {
		Safe_Release(data.second);
	}
	m_textureMap.clear();

	for (auto& data : m_materialMap) {
		Safe_Release(data.second);
	}
	m_materialMap.clear();


	for (auto& data : m_meshMap) {
		Safe_Release(data.second);
	}
	m_meshMap.clear();
}
