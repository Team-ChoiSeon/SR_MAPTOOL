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

	return -1; // 못 찾았을 경우
}

int CResourceMgr::Get_MaterialID(const string& name)
{
	auto it = find_if(m_materialName.begin(), m_materialName.end(),
		[&name](const string& matName) {
			return matName == name;
		});

	if (it != m_materialName.end())
		return static_cast<int>(distance(m_materialName.begin(), it));

	return -1; // 못 찾았을 경우
}

int CResourceMgr::Get_TextureID(const string& name)
{
	auto it = find_if(m_textureName.begin(), m_textureName.end(),
		[&name](const string& texName) {
			return texName == name;
		});

	if (it != m_textureName.end())
		return static_cast<int>(distance(m_textureName.begin(), it));

	return -1; // 못 찾았을 경우
}

int CResourceMgr::Get_NormalID(const string& name)
{
	auto it = find_if(m_normalName.begin(), m_normalName.end(),
		[&name](const string& texName) {
			return texName == name;
		});

	if (it != m_normalName.end())
		return static_cast<int>(distance(m_normalName.begin(), it));

	return -1; // 못 찾았을 경우
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
	// 이미 로드된 머티리얼이 있다면 재사용 (중복 방지)
	auto iter = m_materialMap.find(mtlPath);
	if (iter != m_materialMap.end())
		return iter->second;

	//  기준 루트 경로 설정
	string BasePath = "../../Resource/Material/";
	string filePath = BasePath + mtlPath;
	string mtlKey = mtlPath;

	// .mtl 파일 열기
	//ifstream in(filePath);
	ifstream in;

	in.open(filePath);

	if (!in.is_open()) {
		//MSG_BOX(L"머티리얼 로딩 파일 오류가 생겼습니다. \n 기본 파일로 대체 됩니다.");
		 filePath = BasePath + "Default_A.mtl";
		 // 스트림 상태 초기화
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
	// .mtl 파일 파싱 준비
	string line;
	string matName;
	string texturePath;

	// 한 줄씩 읽으면서 파싱
	while (std::getline(in, line))//한줄씩
	{
		istringstream iss(line);
		string token;
		iss >> token; //line 문자열을 띄어쓰기(공백) 기준으로 토큰 단위로 분리

		// 머티리얼 이름 저장
		if (token == "newmtl") {
			iss >> matName;//newmtl이 나오면 그다음은 머티리얼 네임
		}
		// 디퓨즈 텍스처 경로 추출
		else if (token == "map_Kd") {
			iss >> texturePath;//map_Kd이 나오면 다음은 텍스쳐

			// 경로 문자열에서 마지막 슬래시(/ 또는 \)의 위치를 찾음
			// 예: "C:/folder/texture.jpg" → 마지막 '/'는 index 11
			size_t pos = texturePath.find_last_of("/\\");

			// 찾았다면 (npos가 아니라면) → 파일명만 추출
			// 예: substr(12) → "texture.jpg"
			if (pos != std::string::npos)
				texturePath = texturePath.substr(pos + 1);
		}
	}

	// 텍스처 로드 시 경로 조합 (※ 실제 경로 구조에 따라 수정 가능)
	CTexture* tex = LoadTexture(texturePath); // 경로 수정 필요
	if (!tex) return nullptr;

	// 머티리얼 객체 생성 및 텍스처 설정
	CMaterial* mat = CMaterial::Create();
	mat->Set_MatrialKey(mtlKey);
	//베이스 컬러
	mat->Set_Diffuse(tex);

	// 머티리얼 맵에 등록 (이름으로 저장)
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

