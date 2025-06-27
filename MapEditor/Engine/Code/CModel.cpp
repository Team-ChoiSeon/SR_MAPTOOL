#include "Engine_Define.h"
#include "CModel.h"

CModel::CModel()
{
}

CModel::~CModel()
{
}

CModel* CModel::Create()
{
	return nullptr;
}

HRESULT CModel::Ready_Component()
{
	return S_OK;
}

void CModel::Update_Component(float& dt)
{
}

void CModel::LateUpdate_Component(float& dt)
{
}

CComponent* CModel::Clone() const
{
	return nullptr;
}

void CModel::Free()
{
}
