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
	// 이미 로드된 머티리얼이 있다면 재사용 (중복 방지)
	auto iter = m_materialMap.find(mtlPath);
	if (iter != m_materialMap.end())
		return iter->second;

	// 텍스처 기준 루트 경로 설정 ( 텍스처 파일 경로 조합용)
	string BasePath = "../Bin/Resource/Material/";
	string filePath = BasePath + mtlPath;

	// .mtl 파일 열기
	ifstream in(filePath);
	if (!in.is_open()) {
		MSG_BOX(L"머티리얼 로딩 파일 오류가 생겼습니다");
		return nullptr;
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
	mat->Set_Key(mtlPath);
	mat->Set_Diffuse(tex);
	// 머티리얼 맵에 등록 (이름으로 저장)
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
