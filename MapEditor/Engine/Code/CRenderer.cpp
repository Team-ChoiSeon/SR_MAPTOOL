#include "Engine_Define.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CModel.h"
#include "CGameObject.h"


CRenderer::CRenderer()
{
}

CRenderer::~CRenderer()
{
	Safe_Release(m_pModel);
	Safe_Release(m_pTransform);
}

void CRenderer::Render_Setting()
{
	if (m_pOwner) {
		m_pTransform = m_pOwner->Get_Component<CTransform>();
		m_pTransform->AddRef();

		m_pModel = m_pOwner->Get_Component<CModel>();
		m_pModel->AddRef();
	}
		
}