#include "pch.h"
#include "Engine_Define.h"
#include "CCameraActor.h"

#include "CTransform.h"
#include "CCamera.h"
#include "CInputMgr.h"
#include "CSceneMgr.h"
#include "CFactoryMgr.h"

CCameraActor::CCameraActor()
	:m_pCamera(nullptr),m_pTransform(nullptr),m_eMode(Camera_Mode::Picking)
{
}

CCameraActor::~CCameraActor()
{
}

CCameraActor* CCameraActor::Create()
{
	CCameraActor* instance = new CCameraActor;

	if (FAILED(instance->Ready_GameObject())) {
		Safe_Release(instance);
		instance = nullptr;
	}
	instance->Set_Name("CCameraActor");
	instance->ObjCount = TypeID<CCameraActor>::GetCount();

	return instance;
}

HRESULT CCameraActor::Ready_GameObject()
{
	m_pTransform = Add_Component<CTransform>();
	//m_pTransform->Set_Pos({ 0,0, -5.f });
	m_pCamera = Add_Component<CCamera>();

	return S_OK;
}

void CCameraActor::Update_GameObject(_float& dt)
{
	Update_Component(dt);
}

void CCameraActor::LateUpdate_GameObject(_float& dt)
{
	LateUpdate_Component(dt);
}


void CCameraActor::Free()
{
	Release_Component();
}

REGISTER_OBJECT(CCameraActor)