#include "pch.h"
#include "Engine_Define.h"
#include "CTestCube.h"
#include "CTransform.h"
#include "CModel.h"
#include "CMeshRenderer.h"

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

	return instance;
}

HRESULT CTestCube::Ready_GameObject()
{
	Add_Component<CTransform>();
	Add_Component<CModel>("CUBE");
	CMeshRenderer* renderer = Add_Component<CMeshRenderer>();
	renderer->Render_Setting();

	return S_OK;
}

void CTestCube::Update_GameObject(float& dt)
{
	Update_Component(dt);
}

void CTestCube::LateUpdate_GameObject(float& dt)
{
	LateUpdate_Component(dt);
}

void CTestCube::Free()
{
	Release_Component();
}
