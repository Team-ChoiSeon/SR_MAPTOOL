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

	// [�⺻ ���� ����Ʈ ����]

	ZeroMemory(&m_BaseLight, sizeof(D3DLIGHT9));
	m_BaseLight.Type = D3DLIGHT_DIRECTIONAL;

	// ���� ���� (����ȭ�� ����)
	m_BaseLight.Direction = D3DXVECTOR3(0.f, -1.f, 1.f); // ������ �Ʒ��� �밢��
	m_BaseLight.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);  // ��� ��ǻ��
	m_BaseLight.Specular = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);
	m_BaseLight.Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f); // ���� �ں��Ʈ

	// 0�� ����Ʈ�� ����
	m_pDevice->SetLight(0, &m_BaseLight);
	m_pDevice->LightEnable(0, TRUE);

	return S_OK;
}


void CLightMgr::Update_Light(_float& dt)
{
	// ����Ʈ �� ���� (�ִ� m_MaxSize����)
	m_curMaxSize = min((int)m_LightContainer.size(), m_MaxSize);

	// 0�� ����Ʈ�� �⺻ ����Ʈ�� ���ܵΰ�, 1������ ���
	for (int i = 1; i <= m_MaxSize; ++i)
	{
		if (i <= m_curMaxSize) {
			m_pDevice->SetLight(i, m_LightContainer[i - 1]->Get_Light()); // [0]���� ����
			m_pDevice->LightEnable(i, TRUE);
		}
		else {
			m_pDevice->LightEnable(i, FALSE); // ������ ��Ȱ��ȭ
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

void CLightMgr::Render_Light()
{
	ImGui::DragFloat("##LightX", &m_BaseLight.Direction.x, 0.01f, -1.0f, 1.0f, "%.3f");
	ImGui::DragFloat("##LightY", &m_BaseLight.Direction.y, 0.01f, -1.0f, 1.0f, "%.3f");
	ImGui::DragFloat("##LightZ", &m_BaseLight.Direction.z, 0.01f, -1.0f, 1.0f, "%.3f");
	_vec3 dir = m_BaseLight.Direction;
	D3DXVec3Normalize(&dir, &dir);
	m_BaseLight.Direction = dir;
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

