#include "Engine_Define.h"
#include "CRenderMgr.h"
#include "CRenderer.h"
#include "CResourceMgr.h"

IMPLEMENT_SINGLETON(CRenderMgr)

CRenderMgr::CRenderMgr()
{
}

CRenderMgr::~CRenderMgr()
{
	Free();
}

HRESULT CRenderMgr::Ready_RenderMgr(LPDIRECT3DDEVICE9 pDevice)
{

	return S_OK;
}

void CRenderMgr::Add_Renderer(CRenderer* renderer)
{
	RENDER_PASS pass = renderer->Get_RenderPass();

	auto iter = find_if(m_Renderers[pass].begin(), m_Renderers[pass].end(),
		[&renderer](CRenderer* data)->bool {
			return data == renderer;
		});

	if (iter == m_Renderers[pass].end())
		m_Renderers[pass].push_back(renderer);
}

void CRenderMgr::Remove_Renderer(CRenderer* renderer)
{
	RENDER_PASS pass = renderer->Get_RenderPass();

	auto iter = remove_if(m_Renderers[pass].begin(), m_Renderers[pass].end(),
		[&renderer](CRenderer* data)->bool {
			return data == renderer;
		});

	if (iter != m_Renderers[pass].end()) {
		m_Renderers[pass].erase(iter, m_Renderers[pass].end());
	}
}

void CRenderMgr::Render(LPDIRECT3DDEVICE9 pDevice) //<- 렌더러의 통합을 고려 해보기! 
{
	//렌더 스테이트 설정
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);		// 정규화된 노멀 사용
	pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);			// 스페큘러 하이라이트

	//텍스쳐 설정
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	//pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

 	for (auto& renderer : m_Renderers[RENDER_PASS::RP_SHADOW])
		renderer->Render(pDevice);

	for (auto& renderer : m_Renderers[RENDER_PASS::RP_OPAQUE])
		renderer->Render(pDevice);

	for (auto& renderer : m_Renderers[RENDER_PASS::RP_STENCIL])
		renderer->Render(pDevice);

	for (auto& renderer : m_Renderers[RENDER_PASS::RP_TRANSPARENT])
		renderer->Render(pDevice);

	for (auto& renderer : m_Renderers[RENDER_PASS::RP_UI])
		renderer->Render(pDevice);

	for (auto& renderer : m_Renderers[RENDER_PASS::RP_POSTPROCESS])
		renderer->Render(pDevice);

	Clear();
	//m_StateCache->Clear();
}

void CRenderMgr::Clear()
{
	m_Renderers.clear();
}

void CRenderMgr::Free()
{
}
