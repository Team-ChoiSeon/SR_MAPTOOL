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

}


void CPickable::LateUpdate_Component(_float& dt)
{
	// 현재 마우스 레이 가져오기
	const RAY& nowRay = CPickingMgr::GetInstance()->Get_Ray();

	//로컬 좌표로 변환
	//RAY localRay = RayToLocal(nowRay);
	float distance = 0.f;

	if (ComputeRay_WorldSpace(nowRay, m_pTransform,&distance)) {
		RAYHIT info = { m_pOwner, distance };
		CPickingMgr::GetInstance()->Register_Info(info);
	}
}

void CPickable::Render_Panel(ImVec2 size)
{
	ImGui::SetNextItemOpen(false, ImGuiCond_Once);

	ImGui::Checkbox("##ActivePickable", &m_bActive); ImGui::SameLine();
	ImGui::Text("Valid Pick");
	ImGui::Checkbox("##ActivePickableInGame", &m_bVaildInGame); ImGui::SameLine();
	ImGui::Text("Valid In Game");
}

CComponent* CPickable::Clone() const
{
	return nullptr;
}

void CPickable::Serialize(json& outJson) const
{
		outJson["validInGame"] = m_bVaildInGame;
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
	const AABB aabb = { {-1,-1,-1},{1,1,1} };

	const D3DXVECTOR3 rayOriginLocal = localRay.vRayPos;
	const D3DXVECTOR3& rayDirLocal = localRay.vRayDir;
	//ray.vRayDir = Normalize(ray.vRayDir); // ray 만드는 시점에서 꼭 단위벡터로

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

	D3DXVECTOR3 hitPoint = rayOriginLocal + rayDirLocal * tmin;
	D3DXVECTOR3 length = hitPoint + rayOriginLocal;
	*distance = D3DXVec3Length(&(length));
	return true;
}

_bool CPickable::ComputeRay_WorldSpace(RAY worldRay, CTransform* pTransform, float* distance)
{
	if (!pTransform || !distance)
		return false;

	// 1. 로컬 AABB 기준 정점들
	const D3DXVECTOR3 localVerts[8] = {
		{ -1, -1, -1 }, { 1, -1, -1 }, { -1, 1, -1 }, { 1, 1, -1 },
		{ -1, -1,  1 }, { 1, -1,  1 }, { -1, 1,  1 }, { 1, 1,  1 }
	};

	// 2. 월드 행렬
	const D3DXMATRIX& worldMatrix = pTransform->Get_WorldMatrix();

	// 3. 정점들을 월드로 변환
	D3DXVECTOR3 worldVerts[8];
	for (int i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&worldVerts[i], &localVerts[i], &worldMatrix);

	// 4. AABB min/max 구하기
	D3DXVECTOR3 minPt = worldVerts[0];
	D3DXVECTOR3 maxPt = worldVerts[0];

	for (int i = 1; i < 8; ++i)
	{
		minPt.x = min(minPt.x, worldVerts[i].x);
		minPt.y = min(minPt.y, worldVerts[i].y);
		minPt.z = min(minPt.z, worldVerts[i].z);

		maxPt.x = max(maxPt.x, worldVerts[i].x);
		maxPt.y = max(maxPt.y, worldVerts[i].y);
		maxPt.z = max(maxPt.z, worldVerts[i].z);
	}

	// 5. ray origin, dir (정규화 필수!)
	D3DXVECTOR3 rayOrigin = worldRay.vRayPos;
	D3DXVECTOR3 rayDir = worldRay.vRayDir;
	D3DXVec3Normalize(&rayDir, &rayDir);

	// 6. ray vs AABB 충돌 판정
	float tmin = -FLT_MAX;
	float tmax = FLT_MAX;

	for (int i = 0; i < 3; ++i)
	{
		_float origin = ((_float*)&rayOrigin)[i];
		_float dir = ((_float*)&rayDir)[i];
		_float minB = ((_float*)&minPt)[i];
		_float maxB = ((_float*)&maxPt)[i];

		if (fabs(dir) < 1e-6f)
		{
			if (origin < minB || origin > maxB)
				return false;
		}
		else
		{
			float t1 = (minB - origin) / dir;
			float t2 = (maxB - origin) / dir;

			if (t1 > t2) std::swap(t1, t2);

			tmin = max(tmin, t1);
			tmax = min(tmax, t2);

			if (tmin > tmax)
				return false;
		}
	}

	if (tmax < 0)
		return false;

	// 7. hit 위치 계산 및 거리 저장
	D3DXVECTOR3 hitPoint = rayOrigin + rayDir * tmin;
	D3DXVECTOR3 rayVector = hitPoint - rayOrigin;
	*distance = D3DXVec3Length(&rayVector);

	return true;
}

void CPickable::Free()
{
}

string CPickable::Get_ComponentName() const
{
	return "CPickable";
}
