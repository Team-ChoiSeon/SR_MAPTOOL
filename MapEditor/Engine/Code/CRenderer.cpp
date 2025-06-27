#include "Engine_Define.h"
#include "CRenderer.h"
#include "CGraphicDev.h"
 
IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
	: m_pDevice(nullptr)
{
}

CRenderer::~CRenderer()
{
}

HRESULT CRenderer::Ready_Renderer()
{
	m_pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

	if(!m_pDevice)
		return E_FAIL;

		return S_OK;
}

void CRenderer::Render()
{
	m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CRenderer::Free()
{
}
