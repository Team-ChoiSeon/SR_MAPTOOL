#include "Engine_Define.h"
#include "CEditorSystem.h"
#include "CTransform.h"
#include "CGameObject.h"
#include "CCameraMgr.h"
#include "CCamera.h"
#include "CGraphicDev.h"
#include "CInputMgr.h"
#include "CInputSystem.h"
#include "GUISystem.h"

IMPLEMENT_SINGLETON(CEditorSystem)
CEditorSystem::CEditorSystem()
	: m_selected(nullptr), m_bEditing(false),m_pTransform(nullptr)
{
}

CEditorSystem::~CEditorSystem()
{
}

HRESULT CEditorSystem::Ready_EditorSystem()
{
	m_pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();
	m_pDevice->GetViewport(&vp);
	gizmoOperation = ImGuizmo::TRANSLATE;
	return S_OK;
}
void CEditorSystem::Update_Editor(_float& dt)
{
	if (!m_selected || !m_pTransform) return;

	Key_Check();
}


void CEditorSystem::LateUpdate_Editor(_float& dt)
{
}
void CEditorSystem::Render_Gizmo()
{
	if (!m_selected) return;
	if (CInputSystem* input = m_selected->Get_Component<CInputSystem>()) {
		if (input->KeyBoadr_Control()) 
			return;
	}
	// === ������ ���� ���� ���� ===
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// ī�޶� ���
	_matrix view = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ViewMatrix();
	_matrix proj = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ProjMatrix();
	m_gizmoWorld = m_pTransform->Get_WorldMatrix(); // ���� ������Ʈ ���� ���

	// ����� ����
	ImGuizmo::BeginFrame();
	ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());

	ImGuizmo::SetRect(
		static_cast<float>(vp.X),
		static_cast<float>(vp.Y),
		static_cast<float>(vp.Width),
		static_cast<float>(vp.Height)
	);

	ImGuizmo::Manipulate(
		(float*)&view,
		(float*)&proj,
		gizmoOperation,
		ImGuizmo::LOCAL,
		(float*)&m_gizmoWorld
	);
	if (GUISystem::GetInstance()->UsingUI()) return;

	//// �����
	//ImGui::Text("IsOver: %s", ImGuizmo::IsOver() ? "true" : "false");
	//ImGui::Text("IsUsing: %s", ImGuizmo::IsUsing() ? "true" : "false");
	_vec3 pos, rot, scale;
	DecomposeMatrixToComponents(m_gizmoWorld, &pos, &rot, &scale);

	m_pTransform->Set_Pos(pos);
	//m_pTransform->Set_Rotate(rot);
	m_pTransform->Set_Scale(scale);

	m_bEditing = ImGuizmo::IsUsing();
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

}


void CEditorSystem::Set_Select(CGameObject* obj)
{
	m_selected = obj;
	m_pTransform = obj ? obj->Get_Component<CTransform>() : nullptr;
	m_bEditing = false;

	if (m_pTransform)
		m_gizmoWorld = m_pTransform->Get_WorldMatrix(); // �� �ʱⰪ ����
}

void CEditorSystem::DecomposeMatrixToComponents(const _matrix& mat, _vec3* outPos, _vec3* outRot, _vec3* outScale)
{
	D3DXVECTOR3 scale, pos;
	D3DXQUATERNION dummyQ;

	// pos, scale ����
	if (D3DXMatrixDecompose(&scale, &dummyQ, &pos, &mat) != D3D_OK)
		return;

	*outPos = pos;
	*outScale = scale;

	// ȸ�� ��� �и� (scale ����)
	_matrix rotMat = mat;
	rotMat._11 /= scale.x; rotMat._12 /= scale.x; rotMat._13 /= scale.x;
	rotMat._21 /= scale.y; rotMat._22 /= scale.y; rotMat._23 /= scale.y;
	rotMat._31 /= scale.z; rotMat._32 /= scale.z; rotMat._33 /= scale.z;

	// �и��� ȸ�� ��Ŀ��� ���Ϸ� ����
	_vec3 rot;
	ExtractEulerFromMatrix_XYZ(rotMat, rot);
	*outRot = rot;
}


void CEditorSystem::Key_Check()
{
	if (CInputMgr::GetInstance()->Key_Tap(DIK_T))
		gizmoOperation = ImGuizmo::TRANSLATE;
	if (CInputMgr::GetInstance()->Key_Tap(DIK_R))
		gizmoOperation = ImGuizmo::ROTATE;
	if (CInputMgr::GetInstance()->Key_Tap(DIK_W))
		gizmoOperation = ImGuizmo::SCALE;

}

void CEditorSystem::ExtractEulerFromMatrix_XYZ(const _matrix& mat, _vec3& outRot)
{
	// XYZ ȸ�� ������ ���� ���Ϸ� �� ����
	float pitch = asinf(-mat._31); // X�� (Pitch)

	float yaw = 0.0f;
	float roll = 0.0f;

	if (fabsf(cosf(pitch)) > 1e-6f) {
		yaw = atan2f(mat._21, mat._11);   // Y�� (Yaw)
		roll = atan2f(mat._32, mat._33);   // Z�� (Roll)
	}
	else {
		// ������ ���� (pitch 90��)
		yaw = atan2f(-mat._12, mat._22);
		roll = 0.0f;
	}

	outRot = _vec3(D3DXToDegree(pitch), D3DXToDegree(yaw), D3DXToDegree(roll));
}

void CEditorSystem::Free()
{
}
