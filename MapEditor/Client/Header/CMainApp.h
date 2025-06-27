#pragma once
#include "CBase.h"

namespace Engine {
	class CGraphicDev;
}

class CMainApp : public CBase
{
private:
	explicit CMainApp();
	virtual ~CMainApp();

public: 
	HRESULT Ready_MainApp();
	int Update_MainApp(_float&fTimeDelta);
	void LateUpdate_MainApp(_float&fTimeDelta);
	void Render_MainApp();

private:
	CGraphicDev* m_pDeviceClass;
	LPDIRECT3DDEVICE9		m_pGraphicDev;

public:
	static CMainApp* Create();
private:
	virtual void Free() override;
};
