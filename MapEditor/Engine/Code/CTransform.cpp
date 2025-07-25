#include "Engine_Define.h"
#include "CTransform.h"

CTransform::CTransform()
{
}

CTransform::~CTransform()
{
}

CTransform* CTransform::Create()
{
	CTransform* instance = new CTransform;

	if (FAILED(instance->Ready_Component())) {
		Safe_Release(instance);
		return nullptr;
	}

	return instance;
}

HRESULT CTransform::Ready_Component()
{
	D3DXMatrixIdentity(&m_WorldMat);
	m_vPos = { 0.f,0.f,0.f };
	m_vRotate = { 0.f,0.f,0.f };
	m_vScale = { 1.f,1.f,1.f };
	m_vPivot = { 0.f,0.f,0.f };
	m_vOrbit = { 0.f,0.f,0.f };
	m_vLook = { 0.f, 0.f, 1.f };
	m_vUp = { 0.f, 1.f, 0.f };
	m_vRight = { 1.f, 0.f, 0.f };

	m_pParent = nullptr;

	return S_OK;
}

void CTransform::Update_Component(_float& dt)
{
	D3DXMATRIX matScale, matAxisRot, matRotateX, matRotateY, matRotateZ, matTrans;
	//크기
	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);

	////축회전
	//float angle = D3DXVec3Length(&m_vAxisRotate); // 회전량(벡터 크기)
	//_vec3 axis;
	//D3DXVec3Normalize(&axis, &m_vAxisRotate);     // 회전축
	//D3DXMatrixRotationAxis(&matAxisRot, &axis, angle);
	D3DXMatrixIdentity(&matAxisRot);
	//자전
	D3DXMatrixRotationX(&matRotateX, D3DXToRadian(m_vRotate.x));
	D3DXMatrixRotationY(&matRotateY, D3DXToRadian(m_vRotate.y));
	D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(m_vRotate.z));
	//이동
	D3DXMatrixTranslation(&matTrans, m_vPos.x, m_vPos.y, m_vPos.z);

	m_WorldMat = matScale * matRotateZ  * matRotateY * matRotateX;
	m_WorldMat *= matTrans;
	m_WorldPosMat = matScale * matTrans;
	//-----------------------------------------------------------//
	//공전 
	D3DXMATRIX matToPivot, matFromPivot, matOrbitX, matOrbitY, matOrbitZ;
	D3DXMatrixTranslation(&matFromPivot, -m_vPivot.x, -m_vPivot.y, -m_vPivot.z);
	D3DXMatrixRotationX(&matOrbitX, D3DXToRadian(m_vOrbit.x));
	D3DXMatrixRotationY(&matOrbitY, D3DXToRadian(m_vOrbit.y));
	D3DXMatrixRotationZ(&matOrbitZ, D3DXToRadian(m_vOrbit.z));
	D3DXMatrixTranslation(&matToPivot, m_vPivot.x, m_vPivot.y, m_vPivot.z);

	m_WorldMat *= matFromPivot * matOrbitX * matOrbitY * matOrbitZ * matToPivot;

	//부모
	if (m_pParent) {
		m_WorldMat *= m_pParent->Get_WorldMatrix();
	}

	m_vRight = _vec3(m_WorldMat._11, m_WorldMat._12, m_WorldMat._13);
	m_vUp = _vec3(m_WorldMat._21, m_WorldMat._22, m_WorldMat._23);
	m_vLook = _vec3(m_WorldMat._31, m_WorldMat._32, m_WorldMat._33);

	D3DXVec3Normalize(&m_vRight, &m_vRight);
	D3DXVec3Normalize(&m_vUp, &m_vUp);
	D3DXVec3Normalize(&m_vLook, &m_vLook);

}

void CTransform::LateUpdate_Component(_float& dt)
{


}

CComponent* CTransform::Clone() const
{
	return nullptr;
}

void CTransform::Set_Parent(CTransform* transform)
{
	m_pParent = transform;
}

void CTransform::Set_Pos(_vec3 pos)
{
	m_vPos = pos;
}

void CTransform::Set_Scale(_vec3 scale)
{
	m_vScale = scale;
}

void CTransform::Set_Rotate(_vec3 rot)
{
	m_vRotate = rot;
}

void CTransform::Set_Pivot(_vec3 pivot)
{
	m_vPivot = pivot;
}

void CTransform::Set_Orbit(_vec3 orbit)
{
	m_vOrbit = orbit;
}

void CTransform::Set_Look(_vec3 look)
{
	m_vLook = look;
}

void CTransform::Add_Pos(_vec3 pos)
{
	m_vPos += pos;
}

void CTransform::Add_Scale(_vec3 scale)
{
	m_vScale += scale;
}

void CTransform::Add_Rotate(_vec3 rot)
{
	m_vRotate += rot;
}

void CTransform::Add_Pivot(_vec3 pivot)
{
	m_vPivot += pivot;
}

void CTransform::Add_Orbit(_vec3 orbit)
{
	m_vOrbit += orbit;
}

void CTransform::Add_Axis(_vec3 axis)
{
	m_vAxisRotate += axis;
}

void CTransform::Rotate_Axis(const _vec3& axis, float angle)
{
	if (D3DXVec3LengthSq(&axis) < 0.0001f || angle == 0.f)
		return;

	_matrix matRot;
	D3DXMatrixRotationAxis(&matRot, &axis, angle);

	// 월드 행렬을 축 기준 회전 적용
	m_WorldMat = matRot * m_WorldMat;

	// 방향 벡터도 갱신
	D3DXVec3TransformNormal(&m_vRight, &m_vRight, &matRot);
	D3DXVec3TransformNormal(&m_vUp, &m_vUp, &matRot);
	D3DXVec3TransformNormal(&m_vLook, &m_vLook, &matRot);
}

void CTransform::Set_LookDirection(const _vec3& lookDir)
{
	_vec3 dir = lookDir;
	D3DXVec3Normalize(&dir, &dir);

	// 회전각을 구함
	// Pitch (X 회전) = 위/아래 방향
	// Yaw   (Y 회전) = 좌/우 방향

	float pitch = asinf(-dir.y); // 위아래
	float yaw = atan2f(dir.x, dir.z); // 좌우 (Z-forward 기준)

	m_vRotate.x = D3DXToDegree(pitch);
	m_vRotate.y = D3DXToDegree(yaw);
	// Roll은 보통 0
}


void CTransform::Free()
{
	
}

string CTransform::Get_ComponentName() const
{
	return "CTransform";
}

void CTransform::Render_Panel(ImVec2 size)
{
	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Transform")) // 제목
	{
		float childWidth = ImGui::GetContentRegionAvail().x;
		ImGui::BeginChild("##TransformChild", ImVec2{ childWidth, 180 }, true);
		ImGui::Separator();
		ImGui::Text("Direction Vectors");
		ImGui::InputFloat3("Look", (_float*)&m_vLook, "%.2f", ImGuiInputTextFlags_ReadOnly);
		ImGui::Text("Scale");
		ImGui::Text("X"); ImGui::SameLine();
		ImGui::InputFloat("##Xscale", &m_vScale.x, 0.1f, 1.0f, "%.2f");
		ImGui::Text("Y"); ImGui::SameLine();
		ImGui::InputFloat("##Yscale", &m_vScale.y, 0.1f, 1.0f, "%.2f");
		ImGui::Text("Z"); ImGui::SameLine();
		ImGui::InputFloat("##Zscale", &m_vScale.z, 0.1f, 1.0f, "%.2f");

		ImGui::Separator();

		ImGui::Text("Rotate");
		ImGui::Text("X"); ImGui::SameLine();
		ImGui::InputFloat("##XRotate", &m_vRotate.x, 0.1f, 10.0f, "%.2f");
		ImGui::Text("Y"); ImGui::SameLine();
		ImGui::InputFloat("##YRotate", &m_vRotate.y, 0.1f, 10.0f, "%.2f");
		ImGui::Text("Z"); ImGui::SameLine();
		ImGui::InputFloat("##ZRotate", &m_vRotate.z, 0.1f, 10.0f, "%.2f");

		ImGui::Separator();

		ImGui::Text("Position");
		ImGui::Text("X"); ImGui::SameLine();
		ImGui::InputFloat("##XPosition", &m_vPos.x, 0.1f, 1.0f, "%.2f");
		ImGui::Text("Y"); ImGui::SameLine();
		ImGui::InputFloat("##YPosition", &m_vPos.y, 0.1f, 1.0f, "%.2f");
		ImGui::Text("Z"); ImGui::SameLine();
		ImGui::InputFloat("##ZPosition", &m_vPos.z, 0.1f, 1.0f, "%.2f");

		ImGui::EndChild();
	}
}

void CTransform::Serialize(json& outJson) const
{
		outJson["position"] = { m_vPos.x, m_vPos.y, m_vPos.z };
		outJson["rotation"] = { m_vRotate.x, m_vRotate.y, m_vRotate.z };
		outJson["scale"] = { m_vScale.x, m_vScale.y, m_vScale.z };

		// 필요 시 pivot, orbit 등도 추가 가능
		// outJson["pivot"] = { m_vPivot.x, m_vPivot.y, m_vPivot.z };
}

void CTransform::Deserialize(const json& inJson)
{
	if (inJson.contains("position")) {
		const auto& p = inJson["position"];
		m_vPos = { p[0], p[1], p[2] };
	}

	if (inJson.contains("rotation")) {
		const auto& r = inJson["rotation"];
		m_vRotate = { r[0], r[1], r[2] };
	}

	if (inJson.contains("scale")) {
		const auto& s = inJson["scale"];
		m_vScale = { s[0], s[1], s[2] };
	}

	// pivot, orbit 등도 필요 시 처리 가능
}
