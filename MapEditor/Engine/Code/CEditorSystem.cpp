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
	Free();
}

HRESULT CEditorSystem::Ready_EditorSystem()
{
	m_pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();
	m_pDevice->GetViewport(&vp);
	gizmoOperation = ImGuizmo::TRANSLATE;

	m_pHellyCam = CGameObject::CreateDeafult();
	m_pHellyCam->Add_Component<CTransform>();
	m_pHellyCam->Add_Component<CCamera>();

	return S_OK;
}
void CEditorSystem::Update_Editor(_float& dt)
{
	Sync_MainCam();

	if (CInputMgr::GetInstance()->Key_Down(DIK_9)) {
		m_pHellyCam->Get_Component<CTransform>()->Add_Rotate({ 10,0,0 });
	}
	m_pHellyCam->Update_GameObject(dt);

	if (CInputMgr::GetInstance()->Key_Tap(DIK_F4))
		CRenderMgr::GetInstance()->Set_WireFrame();

	if (CInputMgr::GetInstance()->Key_Tap(DIK_F5))
		CRenderMgr::GetInstance()->Set_CullMode();

	if (CInputMgr::GetInstance()->Key_Tap(DIK_F6))
		CRenderMgr::GetInstance()->Set_LightMode();

	if (!m_selected || !m_pTransform) return;
	Key_Check();
}


void CEditorSystem::LateUpdate_Editor(_float& dt)
{
	m_pHellyCam->LateUpdate_GameObject(dt);
}

void CEditorSystem::Render_Gizmo()
{
	if (!m_selected) return;

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

	//������� ��ġ �̵�
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
	//m_pTransform->Set_Rotate(rot);  // ��׸� �״�� ���
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
		m_gizmoWorld = m_pTransform->Get_WorldMatrix(); 

}

CCamera* CEditorSystem::Get_HellyCame()
{
	if (m_pHellyCam)
		return m_pHellyCam->Get_Component<CCamera>();
	else
		return nullptr;
}

CTransform* CEditorSystem::Get_HellyTrans()
{
	if (m_pHellyCam)
		return m_pHellyCam->Get_Component<CTransform>();
	else
		return nullptr;
}

void CEditorSystem::Sync_MainCam()
{
	if (CCamera* mainCam = CCameraMgr::GetInstance()->Get_MainCamera()) {
		CTransform* camTrans = m_pHellyCam->Get_Component<CTransform>();
		_vec3  vMainCamPos = mainCam->Get_Eye();
		camTrans->Set_Pos({ vMainCamPos.x, vMainCamPos.y + 10, vMainCamPos.z });
		_vec3 vMyPos = camTrans->Get_Pos();
		_vec3 vLook = vMainCamPos -vMyPos;
		camTrans->Set_LookDirection(vLook);
	}
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
	Safe_Release(m_pHellyCam);
}
