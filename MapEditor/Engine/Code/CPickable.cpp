#include "Engine_Define.h"
#include "CPickable.h"
#include "CPickingMgr.h"
#include "CGameObject.h"
#include "CEditorSystem.h"
#include "CTransform.h"
#include "CModel.h"

CPickable::CPickable()
	:m_pTransform(nullptr), m_pModel(nullptr)
{
}

CPickable::~CPickable()
{
}

CPickable* CPickable::Create()
{
	CPickable* instance = new CPickable;

	if (FAILED(instance->Ready_Component())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}


HRESULT CPickable::Ready_Component()
{
	return S_OK;
}

void CPickable::Update_Component(_float& dt)
{
	if (!m_pTransform || !m_pModel) {
		if (m_pOwner) {
			m_pTransform = m_pOwner->Get_Component<CTransform>();
			m_pModel = m_pOwner->Get_Component<CModel>();
		}
	}

	// 현재 마우스 레이 가져오기
	const RAY& nowRay = CPickingMgr::GetInstance()->Get_Ray();

	//로컬 좌표로 변환
	RAY localRay = RayToLocal(nowRay);
	float distance = 0.f;

	if (ComputeRay(localRay, &distance)) {
		RAYHIT info = { m_pOwner, distance };
		CPickingMgr::GetInstance()->Register_Info(info);
	}
}


void CPickable::LateUpdate_Component(_float& dt)
{
}

void CPickable::Render_Panel(ImVec2 size)
{
	ImGui::SetNextItemOpen(false, ImGuiCond_Once);

	ImGui::Checkbox("##ActivePickable", &m_bActive); ImGui::SameLine();
	ImGui::Text("Valid Pick");
}

CComponent* CPickable::Clone() const
{
	return nullptr;
}

void CPickable::Serialize(json& outJson) const
{
}

void CPickable::Deserialize(const json& inJson)
{
}

RAY CPickable::RayToLocal(RAY ray)
{
	RAY localRay;
	if (m_pTransform)
	{
		const D3DXMATRIX& worldMat = m_pTransform->Get_WorldMatrix(); //트랜스폼의 월드 매트릭스

		D3DXMATRIX invWorldMat;
		D3DXMatrixInverse(&invWorldMat, nullptr, &worldMat); // 역행렬 따로 저장

		D3DXVec3TransformCoord(&localRay.vRayPos, &ray.vRayPos, &invWorldMat);  //레이포즈를 로컬 위치 벡터로 변환
		D3DXVec3TransformNormal(&localRay.vRayDir, &ray.vRayDir, &invWorldMat); //레이 디렉션을 로컬 방향 벡터로 변환
		D3DXVec3Normalize(&localRay.vRayDir, &localRay.vRayDir); // 정규화 
	}

	return localRay;
}

_bool CPickable::ComputeRay(RAY localRay, float* distance)
{
	// 오브젝트의 AABB 
	const AABB& aabb = m_pModel->Get_AABB();

	const D3DXVECTOR3& rayOriginLocal = localRay.vRayPos;
	const D3DXVECTOR3& rayDirLocal = localRay.vRayDir;

	const D3DXVECTOR3& localMin = aabb.vMin;
	const D3DXVECTOR3& localMax = aabb.vMax;

	_float  tmin = -FLT_MAX;
	_float  tmax = FLT_MAX;

	for (int i = 0; i < 3; ++i) //각 축별로 계산 중임. (x : 1, y:2, z :3 )
	{
		///광선의 x축 위치와 방향
		_float  origin = ((_float*)&rayOriginLocal)[i];
		_float  dir = ((_float*)&rayDirLocal)[i];

		//박스 민/맥스의 x축 방향
		_float  minB = ((_float*)&localMin)[i];
		_float  maxB = ((_float*)&localMax)[i];

		if (fabs(dir) < 1e-6f) //이게 대축 작다는 소리인 듯		(그리고 0으로 나누면 안되니까)
		{
			if (origin < minB || origin > maxB)	//위치가 min보다 작거나 max보다 크면 (그리고 축 방향성이 0이면)
				return false;											//충돌 안함 -> 왜냐면, 그 안에 있는 것도 아니면서 그 축을 벗어나지 않는다는 뜻이니까
		}
		else
		{
			_float  t1 = (minB - origin) / dir;				//광선 위치에서 min까지 (무슨 방향?? 양 or 음)얼마나 이동해야 하는지
			_float  t2 = (maxB - origin) / dir;				//광선 위치에서 max까지 (무슨 방향?? 양 or 음)얼마나 이동해야 하는지

			if (t1 > t2) swap(t1, t2);							//둘중 작은 거 큰거 바꾸고

			tmin = max(tmin, t1);	// 들어가는 시점 중 가장 늦은 순간(무한이랑 비교해야 가장 늦은 것을 할 수 있음)
			tmax = min(tmax, t2); // 나오는 시점 중 가장 빠른 순간(무한이랑 비교해야 가장 빠른 것을 할 수 있음)


			if (tmin > tmax)
				return false;
		}
	}

	// 광선이 박스 뒤에 있음
	if (tmax < 0)
		return false;

	*distance = tmin;
	return true;
}

void CPickable::Free()
{
}

string CPickable::Get_ComponentName() const
{
	return "CPickable";
}
