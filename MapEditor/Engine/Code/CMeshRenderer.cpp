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

	IMesh* mesh = m_pModel->Get_Mesh();

	pDevice->SetFVF(mesh->GetFVF());

	pDevice->SetStreamSource(0, mesh->Get_VertexBuffer(), 0, sizeof(VTXTILE));
	pDevice->SetIndices(mesh->Get_IndexBuffer());
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
}

CComponent* CMeshRenderer::Clone() const
{
	return nullptr;
}

void CMeshRenderer::Free()
{
}
