#include "pch.h"
#include "Engine_Define.h"
#include "CTestCube.h"
#include "CTransform.h"
#include "CModel.h"
#include "CMeshRenderer.h"

int CTestCube::objCount = 0;

CTestCube::CTestCube()
{
}

CTestCube::~CTestCube()
{
}

CTestCube* CTestCube::Create()
{
	CTestCube* instance = new CTestCube;

	if (FAILED(instance->Ready_GameObject())) {
		Safe_Release(instance);
		instance = nullptr;
	}
	objCount++;
	return instance;
}

HRESULT CTestCube::Ready_GameObject()
{
	m_pTransform = Add_Component<CTransform>();
	m_pTransform->Set_Pos({ 0.f,0.f,20.f });

	Add_Component<CModel>("DirtObj");
	CMeshRenderer* renderer = Add_Component<CMeshRenderer>();
	renderer->Render_Setting();

	return S_OK;
}

void CTestCube::Update_GameObject(_float& dt)
{
	Update_Component(dt);

}

void CTestCube::LateUpdate_GameObject(_float& dt)
{
	LateUpdate_Component(dt);
}

void CTestCube::Free()
{
	Release_Component();
}
