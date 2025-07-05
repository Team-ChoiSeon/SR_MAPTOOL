#include "Engine_Define.h"
#include "CShaderMgr.h"

IMPLEMENT_SINGLETON(CShaderMgr)

CShaderMgr::CShaderMgr()
{
}

CShaderMgr::~CShaderMgr()
{
	Free();
}

HRESULT CShaderMgr::Ready_Shader(LPDIRECT3DDEVICE9 pDevice)
{
	m_pDevice = pDevice;
	return S_OK;
}

LPD3DXEFFECT CShaderMgr::GetShader(const string& path)
{
	auto it = m_mapShader.find(path);

	if (it != m_mapShader.end())
		return it->second;

	LPD3DXEFFECT pEffect = nullptr;
	HRESULT hr = D3DXCreateEffectFromFileA(m_pDevice, path.c_str(), nullptr, nullptr,
		D3DXSHADER_DEBUG, nullptr, &pEffect, nullptr);

	if (FAILED(hr)) { return nullptr; }

	m_mapShader[path] = pEffect;

	return pEffect;
}

HRESULT CShaderMgr::ReloadShader(const string& path)
{
	auto iter = m_mapShader.find(path);
	if (iter != m_mapShader.end()) {
		if (iter->second)
			Safe_Release(iter->second);
		m_mapShader.erase(iter);
	}

	LPD3DXEFFECT shader = GetShader(path);

	if (shader)
		return S_OK;

	return E_FAIL;
}

void CShaderMgr::Free()
{
	for (auto& pair : m_mapShader) {
		Safe_Release(pair.second);
	}
	m_mapShader.clear();
}
