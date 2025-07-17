#include "Engine_Define.h"
#include "CResourceMgr.h"

#include "CGraphicDev.h"
#include "CMesh.h"
#include "CMaterial.h"
#include "CTexture.h"


IMPLEMENT_SINGLETON(CResourceMgr)

CResourceMgr::CResourceMgr()
	:m_pDevice(nullptr)
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
	AutoLoad_OBJMeshes();
	AutoLoad_MaterialFromMTL();
	AutoLoad_Texture();
	AutoLoad_Normal();
	return S_OK;
}
void CResourceMgr::AutoLoad_OBJMeshes()
{
	string folderPath = "../../Resource/Obj/";
	for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
		if (entry.path().extension() == ".obj") {
			string filename = entry.path().filename().string();
			m_meshName.push_back(filename);
		}
	}
}
void CResourceMgr::AutoLoad_MaterialFromMTL()
{
	string folderPath = "../../Resource/Material/";
	for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
		if (entry.path().extension() == ".mtl") {
			string filename = entry.path().filename().string();
			m_materialName.push_back(filename);
		}
	}
}

void CResourceMgr::AutoLoad_Texture()
{
	string folderPath = "../../Resource/Texture/Diffuse/";
	for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
		if (entry.is_regular_file()) {
			string filename = entry.path().filename().string();
			m_textureName.push_back(filename);
		}
	}
}

void CResourceMgr::AutoLoad_Normal()
{
	string folderPath = "../../Resource/Texture/Normal/";
	for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
		if (entry.is_regular_file()) {
			string filename = entry.path().filename().string();
			m_normalName.push_back(filename);
		}
	}
}

int CResourceMgr::Get_MeshID(const string& name)
{
	auto it = find_if(m_meshName.begin(), m_meshName.end(),
		[&name](const string& meshName) {
			return meshName == name;
		});

	if (it != m_meshName.end())
		return static_cast<int>(distance(m_meshName.begin(), it));

	return -1; // �� ã���� ���
}

int CResourceMgr::Get_MaterialID(const string& name)
{
	auto it = find_if(m_materialName.begin(), m_materialName.end(),
		[&name](const string& matName) {
			return matName == name;
		});

	if (it != m_materialName.end())
		return static_cast<int>(distance(m_materialName.begin(), it));

	return -1; // �� ã���� ���
}

int CResourceMgr::Get_TextureID(const string& name)
{
	auto it = find_if(m_textureName.begin(), m_textureName.end(),
		[&name](const string& texName) {
			return texName == name;
		});

	if (it != m_textureName.end())
		return static_cast<int>(distance(m_textureName.begin(), it));

	return -1; // �� ã���� ���
}

int CResourceMgr::Get_NormalID(const string& name)
{
	auto it = find_if(m_normalName.begin(), m_normalName.end(),
		[&name](const string& texName) {
			return texName == name;
		});

	if (it != m_normalName.end())
		return static_cast<int>(distance(m_normalName.begin(), it));

	return -1; // �� ã���� ���
}

CMesh* CResourceMgr::LoadMeshFromOBJ( const std::string& objPath)
{
	auto iter = m_meshMap.find(objPath);
	if (iter != m_meshMap.end()) {
		return iter->second;
	}

	CMesh* mesh = CMesh::Create();
	string meshKey = objPath;

	if (FAILED(mesh->LoadOBJ(m_pDevice, meshKey))) {
		Safe_Release(mesh);
		auto iter = m_meshMap.find(meshKey);
		if (iter != m_meshMap.end()) {
			return iter->second;
		}
		else {
			return nullptr;
		}
	}
	mesh->Set_Key(meshKey);
	m_meshMap[meshKey] = mesh;
	return mesh;
}

CTexture* CResourceMgr::LoadTexture(const std::string& texturePath)
{
	auto it = m_textureMap.find(texturePath);
	if (it != m_textureMap.end()) 
		return it->second;

	string BasePath = "../../Resource/Texture/Diffuse/";
	string filePath;

	if (texturePath.empty())
		filePath = BasePath+ "Grey_Diffuse.PNG";
	else
		 filePath = BasePath + texturePath;

	CTexture* tex = CTexture::Create();

	if (FAILED(tex->Load(m_pDevice, filePath))) {
		Safe_Release(tex);
		return nullptr;
	}
	tex->SetKey(texturePath);
	m_textureMap[texturePath] = tex;

	return tex;
}

CTexture* CResourceMgr::LoadNormal(const string& texturePath)
{
	auto it = m_textureMap.find(texturePath);
	if (it != m_textureMap.end())
		return it->second;

	string BasePath = "../../Resource/Texture/Normal/";
	string filePath;

	if (texturePath.empty())
		filePath = BasePath + "Default_A.PNG";
	else
		filePath = BasePath + texturePath;

	CTexture* tex = CTexture::Create();

	if (FAILED(tex->Load(m_pDevice, filePath))) {
		Safe_Release(tex);
		return nullptr;
	}
	tex->SetKey(texturePath);
	m_normalMap[texturePath] = tex;

	return tex;
}

CMaterial* CResourceMgr::LoadMaterialFromMTL(const std::string& mtlPath)
{
	// �̹� �ε�� ��Ƽ������ �ִٸ� ���� (�ߺ� ����)
	auto iter = m_materialMap.find(mtlPath);
	if (iter != m_materialMap.end())
		return iter->second;

	//  ���� ��Ʈ ��� ����
	string BasePath = "../../Resource/Material/";
	string filePath = BasePath + mtlPath;
	string mtlKey = mtlPath;

	// .mtl ���� ����
	//ifstream in(filePath);
	ifstream in;

	in.open(filePath);

	if (!in.is_open()) {
		//MSG_BOX(L"��Ƽ���� �ε� ���� ������ ������ϴ�. \n �⺻ ���Ϸ� ��ü �˴ϴ�.");
		 filePath = BasePath + "Default_A.mtl";
		 // ��Ʈ�� ���� �ʱ�ȭ
		 in.clear(); 
		 in.open(filePath);
		 if (!in.is_open()) {
			 MessageBoxW(0, L"Mtl Load Err", L"Err", MB_OK);
		 }
		 else {
			 auto iter = m_materialMap.find(mtlKey);
			 if (iter != m_materialMap.end())
				 return iter->second;
		 }
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
	mat->Set_MatrialKey(mtlKey);
	//���̽� �÷�
	mat->Set_Diffuse(tex);

	// ��Ƽ���� �ʿ� ��� (�̸����� ����)
	m_materialMap[mtlKey] = mat;
	return mat;
}

CMesh* CResourceMgr::GetMesh(const std::string& key)
{
	auto iter =m_meshMap.find(key);
	if (iter != m_meshMap.end()) {
		return iter->second;
	}
	else {
		return LoadMeshFromOBJ(key);
	}
}

CTexture* CResourceMgr::GetTexture(const std::string& key)
{
	auto iter = m_textureMap.find(key);
	if (iter != m_textureMap.end()) {
		return iter->second;
	}
	else {
		return LoadTexture(key);
	}
}

CTexture* CResourceMgr::GetNormal(const std::string& key)
{
	auto iter = m_normalMap.find(key);
	if (iter != m_normalMap.end()) {
		return iter->second;
	}
	else {
		return LoadTexture(key);
	}
}

CMaterial* CResourceMgr::GetMaterial(const std::string& key)
{
	auto iter = m_materialMap.find(key);
	if (iter != m_materialMap.end()) {
		return iter->second;
	}
	else {
		return LoadMaterialFromMTL(key);
	}
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

