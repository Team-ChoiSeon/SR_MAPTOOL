#include "Engine_Define.h"
#include "CCameraMgr.h"
#include "CCamera.h"
#include "CGraphicDev.h"
IMPLEMENT_SINGLETON(CCameraMgr)

CCameraMgr::CCameraMgr()
{
}

CCameraMgr::~CCameraMgr()
{
	Free();
}

HRESULT CCameraMgr::Ready_CameraMgr()
{
	m_pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

	return S_OK;
}

void CCameraMgr::Update_Camera(_float& dt)
{
	_matrix& viewMat = m_pMainCam->Get_ViewMatrix();
	_matrix& projMat = m_pMainCam->Get_ProjMatrix();

	m_pDevice->SetTransform(D3DTS_VIEW, &viewMat);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &projMat);
}

void CCameraMgr::LateUpdate_Camera(_float& dt)
{
}

void CCameraMgr::Set_MainCamera(CCamera* pCam)
{
	m_pMainCam = pCam;
}

void CCameraMgr::Free()
{
}
