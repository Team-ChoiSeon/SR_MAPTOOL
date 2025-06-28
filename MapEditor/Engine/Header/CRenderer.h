#pragma once
#include "CComponent.h"
BEGIN(Engine)
class CTransform;
class CModel;

class ENGINE_DLL CRenderer :
    public CComponent
{
protected:
    explicit CRenderer();
   virtual ~CRenderer();
public:
	virtual HRESULT Ready_Component() PURE;
	virtual void Render(LPDIRECT3DDEVICE9 pDevice) PURE;
	virtual CComponent* Clone() const PURE;
	virtual RENDER_PASS Get_RenderPass() PURE;
	virtual void Render_Setting();

protected:
	CTransform* m_pTransform;
	CModel* m_pModel;

private:
    void Free() override PURE;
};

END