#include "Engine_Define.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CGameObject.h"


void CRenderer::Free()
{
}

CRenderer::CRenderer()
{
}

CRenderer::~CRenderer()
{
}

void CRenderer::Set_Transform()
{
	if (m_pOwner)
		m_pTransform = m_pOwner->Get_Component<CTransform>();
}