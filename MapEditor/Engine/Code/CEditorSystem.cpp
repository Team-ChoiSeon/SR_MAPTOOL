#include "Engine_Define.h"
#include "CEditorSystem.h"
#include "CTransform.h"
#include "CGameObject.h"
#include "CCameraMgr.h"
#include "CCamera.h"
#include "CGraphicDev.h"
#include "CInputMgr.h"
#include "CInputSystem.h"
#include "CSceneMgr.h"
#include "GUISystem.h"
#include "CRenderMgr.h"
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
	if (CInputMgr::GetInstance()->Key_Tap(DIK_F4))
		CRenderMgr::GetInstance()->Set_WireFrame();

	if (CInputMgr::GetInstance()->Key_Tap(DIK_F5))
		CRenderMgr::GetInstance()->Set_CullMode();

	if (!m_selected || !m_pTransform) return;
	Key_Check();
}


void CEditorSystem::LateUpdate_Editor(_float& dt)
{
	//m_selected = CSceneMgr::GetInstance()->Get_SelectedObject();
	//Set_Select(m_selected);
}

void CEditorSystem::Render_Gizmo()
{
	if (!m_selected) return;

	// === 기즈모용 렌더 상태 설정 ===
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// 카메라 행렬
	_matrix view = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ViewMatrix();
	_matrix proj = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ProjMatrix();
	m_gizmoWorld = m_pTransform->Get_WorldMatrix(); // 현재 오브젝트 월드 행렬

	// 기즈모 세팅
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

	//여기부터 위치 이동
	if (GUISystem::GetInstance()->UsingUI()) return;
	if (CCameraMgr::GetInstance()->Is_DefaultCam() && CCameraMgr::GetInstance()->Is_Moving()) return;
	if (m_selected)
	{
		if (m_pInputSystem)
		{
			if (m_pInputSystem->KeyBoadr_Control())
				return;
		}
	}
	
	_vec3 pos, rot, scale;
	ImGuizmo::DecomposeMatrixToComponents(m_gizmoWorld, &pos.x, &rot.x, &scale.x);

	m_pTransform->Set_Pos(pos);
	m_pTransform->Set_Rotate(rot);  // 디그리 그대로 사용
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
	m_pInputSystem = obj ? obj->Get_Component<CInputSystem>() : nullptr;

	m_bEditing = obj ? true : false;
	if (m_pTransform)
		m_gizmoWorld = m_pTransform->Get_WorldMatrix(); // ← 초기값 복사

}

void CEditorSystem::Key_Check()
{
	if (CInputMgr::GetInstance()->Key_Tap(DIK_F1))
		gizmoOperation = ImGuizmo::TRANSLATE;
	if (CInputMgr::GetInstance()->Key_Tap(DIK_F2))
		gizmoOperation = ImGuizmo::ROTATE;
	if (CInputMgr::GetInstance()->Key_Tap(DIK_F3))
		gizmoOperation = ImGuizmo::SCALE;

}

void CEditorSystem::Free()
{
}
