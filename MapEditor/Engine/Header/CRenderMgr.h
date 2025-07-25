#pragma once
#include "CBase.h"

BEGIN(Engine)
class CModel;
class CParticle;
class CGhostModel;
class CCamera;

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
	void Render_GUI();
public:
	void Add_ModelRenderer(CModel* renderer);
	void Add_ParticleRenderer(CParticle* particle);
	void Add_GhostRenderer(CGhostModel* particle);
	void Remove_Renderer(CModel* renderer);
	CCamera* Get_RenderCam() { return m_pRenderCam; }
	void Set_WireFrame() { m_bWireFram = (!m_bWireFram); };
	void Set_CullMode() { m_iCullMode++; if (m_iCullMode > 2) m_iCullMode = 0; };
	void Set_LightMode() { m_bLightMode = (!m_bLightMode); };

	_bool Get_LightMode() { return m_bLightMode; }
	void Clear();

private:
	unordered_map<RENDER_PASS, vector<CModel*>> m_Models;
	vector<CParticle*> m_Particles;
	vector<CGhostModel*> m_Ghosts;

	_bool m_bWireFram = false;
	_bool m_bLightMode = false;
	_uint m_iCullMode = 0;

	LPDIRECT3DSURFACE9 g_pBackBuffer = nullptr;
	LPDIRECT3DTEXTURE9 g_pMyTexture = nullptr;

	CCamera* m_pRenderCam;
private:
	virtual void Free() override;
};

END