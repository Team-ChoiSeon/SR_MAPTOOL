#include "Engine_Define.h"
#include "CLightMgr.h"
#include "CLight.h"
#include "CCameraMgr.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CCamera.h"

IMPLEMENT_SINGLETON(CLightMgr)

CLightMgr::CLightMgr()
{
}

CLightMgr::~CLightMgr()
{
}

HRESULT CLightMgr::Ready_Light(LPDIRECT3DDEVICE9 pDevice)
{
	m_pDevice = pDevice;
	return S_OK;
}

void CLightMgr::Update_Light(_float& dt)
{
	// 라이트 수 조정 (최대 m_MaxSize까지)
	m_curMaxSize = min((int)m_LightContainer.size(), m_MaxSize);

	// 0번 라이트는 기본 라이트로 남겨두고, 1번부터 사용
	for (int i = 1; i <= m_MaxSize; ++i)
	{
		if (i <= m_curMaxSize) {
			m_pDevice->SetLight(i, m_LightContainer[i - 1]->Get_Light()); // [0]부터 시작
			m_pDevice->LightEnable(i, TRUE);
		}
		else {
			m_pDevice->LightEnable(i, FALSE); // 나머지 비활성화
		}
	}
}


void CLightMgr::LateUpdate_Light(_float& dt)
{
	Sort_Light();
}

void CLightMgr::Add_Light(CLight* light)
{
	m_LightContainer.push_back(light);
}

void CLightMgr::Remove_Light(CLight* light)
{
	m_LightContainer.erase(
		remove_if(
			m_LightContainer.begin(), m_LightContainer.end(),
			[&light](CLight* data)->bool {
				return light == data;
			}
		)
	);
}
void CLightMgr::Sort_Light()
{
	_vec3 CamPos = CCameraMgr::GetInstance()->Get_MainCamera()->Get_Eye();


	std::partial_sort(m_LightContainer.begin(), m_LightContainer.begin() + m_curMaxSize,  m_LightContainer.end(),
		[&](CLight* a, CLight* b) {
			return DistanceSqare(a->Get_Pos(), CamPos) < DistanceSqare(b->Get_Pos(), CamPos);
		});

}

float CLightMgr::DistanceSqare(_vec3 light, _vec3 cam)
{
	_vec3 diff = light - cam;
	return D3DXVec3LengthSq(&diff); 
}

void CLightMgr::Free()
{
	m_LightContainer.clear();
}

