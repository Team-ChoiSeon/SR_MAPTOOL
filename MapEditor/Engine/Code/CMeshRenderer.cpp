#include "Engine_Define.h"
#include "CMeshRenderer.h"
#include "CResourceMgr.h"
#include "CRenderMgr.h"
#include "CTransform.h"
#include "CModel.h"
#include "IMesh.h"

CMeshRenderer::CMeshRenderer()
{
}

CMeshRenderer::~CMeshRenderer()
{
}

CMeshRenderer* CMeshRenderer::Create()
{
	CMeshRenderer* instance = new CMeshRenderer;

	if (FAILED(instance->Ready_Component())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CMeshRenderer::Ready_Component()
{
	return S_OK;
}

void CMeshRenderer::Update_Component(float& dt)
{

}

void CMeshRenderer::LateUpdate_Component(float& dt)
{
	if (m_bActive)
		CRenderMgr::GetInstance()->Add_Renderer(this);
}

void CMeshRenderer::Render(LPDIRECT3DDEVICE9 pDevice)
{
	if (!m_pTransform || !m_pModel)
		return;

	// Transform Àû¿ë
	pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_WorldMatrix());
	m_pModel->Render(pDevice);

}

CComponent* CMeshRenderer::Clone() const
{
	return nullptr;
}

void CMeshRenderer::Free()
{
}
