#pragma once
#include "CBase.h"

BEGIN(Engine)
class CModel;

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
	void Add_Renderer(CModel* renderer);
	void Remove_Renderer(CModel* renderer);
	void Clear();

private:
	unordered_map<RENDER_PASS, list<CModel*>> m_Renderers;

private:
	virtual void Free() override;
};

END