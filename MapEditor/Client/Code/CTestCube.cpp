#include "pch.h"
#include "Engine_Define.h"
#include "CTestCube.h"
#include "CTransform.h"
#include "CModel.h"

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

	return S_OK;
}

void CTestCube::Update_GameObject(float& dt)
{
}

void CTestCube::LateUpdate_GameObject(float& dt)
{
}

void CTestCube::Free()
{
}
