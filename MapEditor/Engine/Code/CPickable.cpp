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
	// ���� ���콺 ���� ��������
	const RAY& nowRay = CPickingMgr::GetInstance()->Get_Ray();

	//���� ��ǥ�� ��ȯ
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
		const D3DXMATRIX& worldMat = m_pTransform->Get_WorldMatrix(); //Ʈ�������� ���� ��Ʈ����

		D3DXMATRIX invWorldMat;
		D3DXMatrixInverse(&invWorldMat, nullptr, &worldMat); // ����� ���� ����

		D3DXVec3TransformCoord(&localRay.vRayPos, &ray.vRayPos, &invWorldMat);  //������� ���� ��ġ ���ͷ� ��ȯ
		D3DXVec3TransformNormal(&localRay.vRayDir, &ray.vRayDir, &invWorldMat); //���� �𷺼��� ���� ���� ���ͷ� ��ȯ
		D3DXVec3Normalize(&localRay.vRayDir, &localRay.vRayDir); // ����ȭ 
	}

	return localRay;
}

_bool CPickable::ComputeRay(RAY localRay, float* distance)
{
	// ������Ʈ�� AABB 
	const AABB aabb = { {-1,-1,-1},{1,1,1} };

	const D3DXVECTOR3 rayOriginLocal = localRay.vRayPos;
	const D3DXVECTOR3& rayDirLocal = localRay.vRayDir;
	//ray.vRayDir = Normalize(ray.vRayDir); // ray ����� �������� �� �������ͷ�

	const D3DXVECTOR3& localMin = aabb.vMin;
	const D3DXVECTOR3& localMax = aabb.vMax;

	_float  tmin = -FLT_MAX;
	_float  tmax = FLT_MAX;

	for (int i = 0; i < 3; ++i) //�� �ະ�� ��� ����. (x : 1, y:2, z :3 )
	{
		///������ x�� ��ġ�� ����
		_float  origin = ((_float*)&rayOriginLocal)[i];
		_float  dir = ((_float*)&rayDirLocal)[i];

		//�ڽ� ��/�ƽ��� x�� ����
		_float  minB = ((_float*)&localMin)[i];
		_float  maxB = ((_float*)&localMax)[i];

		if (fabs(dir) < 1e-6f) //�̰� ���� �۴ٴ� �Ҹ��� ��		(�׸��� 0���� ������ �ȵǴϱ�)
		{
			if (origin < minB || origin > maxB)	//��ġ�� min���� �۰ų� max���� ũ�� (�׸��� �� ���⼺�� 0�̸�)
				return false;											//�浹 ���� -> �ֳĸ�, �� �ȿ� �ִ� �͵� �ƴϸ鼭 �� ���� ����� �ʴ´ٴ� ���̴ϱ�
		}
		else
		{
			_float  t1 = (minB - origin) / dir;				//���� ��ġ���� min���� (���� ����?? �� or ��)�󸶳� �̵��ؾ� �ϴ���
			_float  t2 = (maxB - origin) / dir;				//���� ��ġ���� max���� (���� ����?? �� or ��)�󸶳� �̵��ؾ� �ϴ���

			if (t1 > t2) swap(t1, t2);							//���� ���� �� ū�� �ٲٰ�

			tmin = max(tmin, t1);	// ���� ���� �� ���� ���� ����(�����̶� ���ؾ� ���� ���� ���� �� �� ����)
			tmax = min(tmax, t2); // ������ ���� �� ���� ���� ����(�����̶� ���ؾ� ���� ���� ���� �� �� ����)


			if (tmin > tmax)
				return false;
		}
	}

	// ������ �ڽ� �ڿ� ����
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

	// 1. ���� AABB ���� ������
	const D3DXVECTOR3 localVerts[8] = {
		{ -1, -1, -1 }, { 1, -1, -1 }, { -1, 1, -1 }, { 1, 1, -1 },
		{ -1, -1,  1 }, { 1, -1,  1 }, { -1, 1,  1 }, { 1, 1,  1 }
	};

	// 2. ���� ���
	const D3DXMATRIX& worldMatrix = pTransform->Get_WorldMatrix();

	// 3. �������� ����� ��ȯ
	D3DXVECTOR3 worldVerts[8];
	for (int i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&worldVerts[i], &localVerts[i], &worldMatrix);

	// 4. AABB min/max ���ϱ�
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

	// 5. ray origin, dir (����ȭ �ʼ�!)
	D3DXVECTOR3 rayOrigin = worldRay.vRayPos;
	D3DXVECTOR3 rayDir = worldRay.vRayDir;
	D3DXVec3Normalize(&rayDir, &rayDir);

	// 6. ray vs AABB �浹 ����
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

	// 7. hit ��ġ ��� �� �Ÿ� ����
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
