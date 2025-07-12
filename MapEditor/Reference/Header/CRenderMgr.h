#pragma once
#include "CBase.h"

BEGIN(Engine)
class CModel;
class CParticle;

class ENGINE_DLL CRenderMgr :
	public CBase
{
	DECLARE_SINGLETON(CRenderMgr)

private:
	explicit CRenderMgr();
	virtual ~CRenderMgr();

public:
	HRESULT Ready_RenderMgr(LPDIRECT3DDEVICE9 pDevice);
	void Render(LPDIRECT3DDEVICE9 pDevice);

public:
	void Add_ModelRenderer(CModel* renderer);
	void Add_ParticleRenderer(CParticle* particle);
	void Remove_Renderer(CModel* renderer);
	void Set_WireFrame() { WireFram = (!WireFram); };
	void Clear();

private:
	unordered_map<RENDER_PASS, vector<CModel*>> m_Models;
	vector<CParticle*> m_Particles;
	_bool WireFram = false;
private:
	virtual void Free() override;
};

END