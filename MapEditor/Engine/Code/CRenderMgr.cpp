#include "Engine_Define.h"
#include "CRenderMgr.h"
#include "CResourceMgr.h"
#include "CModel.h"
#include "CParticle.h"
#include "CGhostModel.h"

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

void CRenderMgr::Add_ModelRenderer(CModel* renderer)
{
	RENDER_PASS pass = renderer->Get_RenderPass();

	auto iter = find_if(m_Models[pass].begin(), m_Models[pass].end(),
		[&renderer](CModel* data)->bool {
			return data == renderer;
		});

	if (iter == m_Models[pass].end())
		m_Models[pass].push_back(renderer);
}

void CRenderMgr::Add_ParticleRenderer(CParticle* particle)
{
	auto iter = find_if(m_Particles.begin(), m_Particles.end(),
		[&particle](CParticle* data)->bool {
			return data == particle;
		});

	if (iter == m_Particles.end())
		m_Particles.push_back(particle);
}

void CRenderMgr::Add_GhostRenderer(CGhostModel* particle)
{
	auto iter = find_if(m_Ghosts.begin(), m_Ghosts.end(),
		[&particle](CGhostModel* data)->bool {
			return data == particle;
		});

	if (iter == m_Ghosts.end())
		m_Ghosts.push_back(particle);
}

void CRenderMgr::Remove_Renderer(CModel* renderer)
{
	RENDER_PASS pass = renderer->Get_RenderPass();

	auto iter = remove_if(m_Models[pass].begin(), m_Models[pass].end(),
		[&renderer](CModel* data)->bool {
			return data == renderer;
		});

	if (iter != m_Models[pass].end()) {
		m_Models[pass].erase(iter, m_Models[pass].end());
	}
}

void CRenderMgr::Render(LPDIRECT3DDEVICE9 pDevice)
{
	//렌더 스테이트 설정
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_LIGHTING, m_bLightMode);

	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);		// 정규화된 노멀 사용
	pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);			// 스페큘러 하이라이트

	//텍스쳐 설정
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	if(m_bWireFram)
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);


	if(m_iCullMode == 0 )
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	else if(m_iCullMode == 1 )
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	else if(m_iCullMode == 2 )
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	for (auto& renderer : m_Models[RENDER_PASS::RP_SHADOW])
		renderer->Render(pDevice);

	for (auto& renderer : m_Models[RENDER_PASS::RP_OPAQUE])
		renderer->Render(pDevice);

	for (auto& renderer : m_Ghosts)
		renderer->Render(pDevice);

	for (auto& renderer : m_Models[RENDER_PASS::RP_STENCIL])
		renderer->Render(pDevice);

	for (auto& renderer : m_Particles)
		renderer->Render_Particle(pDevice);

	for (auto& renderer : m_Models[RENDER_PASS::RP_TRANSPARENT])
		renderer->Render(pDevice);

	for (auto& renderer : m_Models[RENDER_PASS::RP_UI])
		renderer->Render(pDevice);

	for (auto& renderer : m_Models[RENDER_PASS::RP_POSTPROCESS])
		renderer->Render(pDevice);

	Clear();
}

void CRenderMgr::Clear()
{
	m_Models.clear();
	m_Particles.clear();
	m_Ghosts.clear();
}

void CRenderMgr::Free()
{
}
