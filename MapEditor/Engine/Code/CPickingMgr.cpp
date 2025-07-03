#include "Engine_Define.h"
#include "CPickingMgr.h"
#include "CGraphicDev.h"
#include "CInputMgr.h"
#include "CSceneMgr.h"
#include "GUISystem.h"
#include "CCameraMgr.h"

#include "CScene.h"
#include "CLayer.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CCamera.h"
#include "CModel.h"
#include "CMesh.h"


IMPLEMENT_SINGLETON(CPickingMgr)

CPickingMgr::CPickingMgr()
	:m_pDevice(nullptr), m_pTarget(nullptr)
{
}

CPickingMgr::~CPickingMgr()
{
	Free();
}

HRESULT CPickingMgr::Ready_Picking(HWND g_hWnd)
{
	m_pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();
	if (!m_pDevice) {
		return E_FAIL;
	}
	m_pDevice->AddRef();
	m_hwnd = g_hWnd;
	return S_OK;
}

void CPickingMgr::Update_Picking(_float& dt)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hwnd, &pt);  // ��ũ�� �� Ŭ���̾�Ʈ
	m_CursorPos = pt;

	if (CInputMgr::GetInstance()->Mouse_Down(DIM_RB))  
		m_pTarget =nullptr;

	Mode_Check(dt);
	GUISystem::GetInstance()->RegisterPanel("Mode", [this]() {Mode_Panel();});

	if (!GUISystem::GetInstance()->UsingUI()) {
		ComputeRay();
		if(!m_pTarget)
			Start_RayCasting();
		if (CInputMgr::GetInstance()->Mouse_Hold(DIM_LB)) {
			if (m_eMode == Screen_Mode::ObjectMode)
				MoveObject(dt);
			else if (m_eMode == Screen_Mode::CamMode)
				MoveCamera(dt);
		}
	}

}

void CPickingMgr::LateUpdate_Picking(_float& dt)
{
}

void CPickingMgr::ComputeRay()
{
	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9)); //dx ����Ʈ ��������
	m_pDevice->GetViewport(&ViewPort);

	_vec3 ndc;
	//NDC ��ǥ�� ��ȯ
	// ���� x�� 600�̶�� (����Ʈ �� ���� - ���콺 ������ / ����Ʈ ����) 
	//�̷��� 0���� 1���� ���̿� ��� ��ġ������ ����.
	//�׶� 2�� ���ϸ� 0���� 2���� ���̿� ��� ��ġ������ ����
	//�̶� 1�� ���� -1~1���̿� ��ġ������ ����.
	ndc.x = ((float)m_CursorPos.x / (float)ViewPort.Width) * 2.0f - 1.0f;
	ndc.y = 1.0f - ((float)m_CursorPos.y / (float)ViewPort.Height) * 2.0f;
	ndc.z = 1.0f;

	// ���� -> �� �����̽�
	D3DXMATRIX	matProj;
	matProj = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ProjMatrix();
	D3DXMatrixInverse(&matProj, 0, &matProj); //����� ������.
	D3DXVec3TransformCoord(&ndc, &ndc, &matProj); //�� ������� �������� ndc�� �����.
	//�̷��� �� �����̽��� �����Ե� (�� ī�޶� �������� ���� �� ����)

	// �� �����̽� -> ����
	D3DXMATRIX	matView;
	matView = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ViewMatrix();
	D3DXMatrixInverse(&matView, 0, &matView);

	m_tRay.vRayPos = { 0.f, 0.f, 0.f }; //�̰� ī�޶��� ��ġ��. �ֳĸ�, ī�޶�� �������� ������ z1�� �ٶ󺸴ϱ�(�佺���̽� ��Ȳ)
	//Ŭ���� �ϰ� �Ǹ� ī�޶󿡼� �����°Ŵϱ�-> ī�޶� ��ġ���� ������ ������ �Ǵ� ���� �´�.
	m_tRay.vRayDir = ndc - m_tRay.vRayPos; //�׷��⿡ ���� ī�޶󿡼� ndc�� ���ϴ� ������ ���Ѵ�

	D3DXVec3TransformCoord(&m_tRay.vRayPos, &m_tRay.vRayPos, &matView); //������� ���� ��ġ ���ͷ� ��ȯ
	D3DXVec3TransformNormal(&m_tRay.vRayDir, &m_tRay.vRayDir, &matView); //���� �𷺼��� ���� ���� ���ͷ� ��ȯ
	D3DXVec3Normalize(&m_tRay.vRayDir, &m_tRay.vRayDir);

}

RAY CPickingMgr::RayToLocal(CTransform* transform)
{
	RAY localRay;

	const D3DXMATRIX& worldMat = transform->Get_WorldMatrix(); //Ʈ�������� ���� ��Ʈ����

	D3DXMATRIX invWorldMat;
	D3DXMatrixInverse(&invWorldMat, nullptr, &worldMat); // ����� ���� ����

	D3DXVec3TransformCoord(&localRay.vRayPos, &m_tRay.vRayPos, &invWorldMat);  //������� ���� ��ġ ���ͷ� ��ȯ
	D3DXVec3TransformNormal(&localRay.vRayDir, &m_tRay.vRayDir, &invWorldMat); //���� �𷺼��� ���� ���� ���ͷ� ��ȯ
	D3DXVec3Normalize(&localRay.vRayDir, &localRay.vRayDir); // ����ȭ 

	return localRay;
}

void CPickingMgr::Start_RayCasting()
{
	if (!CInputMgr::GetInstance()->Mouse_Down(DIM_LB))  return;

	CScene* nowScene = CSceneMgr::GetInstance()->Get_NowScene();

	_float distance = FLT_MAX; //�Ÿ� �켱 ���� ��� 
	CGameObject* pPickedObject = nullptr; //��ŷ�� ������� �־��� ����

	int max = static_cast<int>(LAYER_ID::L_END);
	for (int i = 0; i < max; ++i) {
		LAYER_ID id = static_cast<LAYER_ID>(i);
		CLayer* tmpLayer = nowScene->Get_Layer(id);
		if (!tmpLayer) continue;

		vector<CGameObject*>& objVector = tmpLayer->Get_Object();

		if (objVector.empty()) continue;

		for (CGameObject* obj : objVector) {
			_float nowDistance = Calc_ObjRay(obj);
			if (nowDistance < 0) continue; //�������� ������ ����

			if (distance > nowDistance) {
				distance = nowDistance;
				pPickedObject = obj;
			}
		}
	}

	if(pPickedObject)
		m_pTarget = pPickedObject;
}

_float CPickingMgr::Calc_ObjRay(CGameObject* obj)
{
	// ����: ���� �� ���� ��ȯ
	CTransform* transform = obj->Get_Component<CTransform>();
	if (!transform) return -1.f;

	RAY localRay = RayToLocal(transform);

	CModel* model = obj->Get_Component<CModel>();
	if (!model) return -1.f;

	CMesh* mesh = model->Get_Mesh();
	if (!mesh) return -1.f;

	AABB objBox = mesh->Get_AABBBOX();

	const D3DXVECTOR3& rayOriginLocal = localRay.vRayPos;
	const D3DXVECTOR3& rayDirLocal = localRay.vRayDir;

	const D3DXVECTOR3& localMin = objBox.vMin;
	const D3DXVECTOR3& localMax = objBox.vMax;

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
				return -1.f;											//�浹 ���� -> �ֳĸ�, �� �ȿ� �ִ� �͵� �ƴϸ鼭 �� ���� ����� �ʴ´ٴ� ���̴ϱ�
		}
		else
		{
			_float  t1 = (minB - origin) / dir;				//���� ��ġ���� min���� (���� ����?? �� or ��)�󸶳� �̵��ؾ� �ϴ���
			_float  t2 = (maxB - origin) / dir;				//���� ��ġ���� max���� (���� ����?? �� or ��)�󸶳� �̵��ؾ� �ϴ���

			if (t1 > t2) swap(t1, t2);							//���� ���� �� ū�� �ٲٰ�

			tmin = max(tmin, t1);	// ���� ���� �� ���� ���� ����(�����̶� ���ؾ� ���� ���� ���� �� �� ����)
			tmax = min(tmax, t2); // ������ ���� �� ���� ���� ����(�����̶� ���ؾ� ���� ���� ���� �� �� ����)


			if (tmin > tmax)
				return -1.f;
		}
	}

	// ������ �ڽ� �ڿ� ����
	if (tmax < 0)
		return -1.f;

	// ���� �Ÿ� ��ȯ
	return tmin;
}

void CPickingMgr::Mode_Check(_float& dt)
{
	if (CInputMgr::GetInstance()->Key_Down(DIK_F1))
		m_eMode = Screen_Mode::CamMode;
	if (CInputMgr::GetInstance()->Key_Down(DIK_F2))
		m_eMode = Screen_Mode::ObjectMode;
	if (CInputMgr::GetInstance()->Key_Down(DIK_Q))
		m_eMove = Move_Mode::Transform;
	if (CInputMgr::GetInstance()->Key_Down(DIK_W))
		m_eMove = Move_Mode::Scale;
	if (CInputMgr::GetInstance()->Key_Down(DIK_E))
		m_eMove = Move_Mode::Rotate;
}

void CPickingMgr::Mode_Panel()
{
	ImGui::Begin("Edit Mode", nullptr, 0);
	ImVec2 Btn = { 90, 20 };

	// --- Screen Mode ---
	ImGui::Text("Screen Mode");

	bool isCam = (m_eMode == Screen_Mode::CamMode);
	if (ImGui::Selectable("F1 : Cam", isCam, ImGuiSelectableFlags_DontClosePopups,
		Btn))  // ũ�� ����
		m_eMode = Screen_Mode::CamMode;
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("ī�޶� �̵� �� ȸ�� ����Դϴ�.");

	ImGui::SameLine();

	bool isObject = (m_eMode == Screen_Mode::ObjectMode);
	if (ImGui::Selectable("F2 : Object", isObject, ImGuiSelectableFlags_DontClosePopups,
		Btn))
		m_eMode = Screen_Mode::ObjectMode;
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("������Ʈ�� �����ϰų� ������ �� �ִ� ����Դϴ�.");

	ImGui::Separator();

	// --- Move Mode ---
	ImGui::Text("Move Mode");
	bool isMove = (m_eMove == Move_Mode::Transform);
	if (ImGui::Selectable("Q : Move", isMove, ImGuiSelectableFlags_DontClosePopups,
		Btn))
		m_eMove = Move_Mode::Transform;
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("������Ʈ�� �̵���ŵ�ϴ�.");

	ImGui::SameLine();

	bool isScale = (m_eMove == Move_Mode::Scale);
	if (ImGui::Selectable("W : Scale", isScale, ImGuiSelectableFlags_DontClosePopups,
		Btn))
		m_eMove = Move_Mode::Scale;
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("������Ʈ�� ũ�⸦ �����մϴ�.");

	ImGui::SameLine();

	bool isRotate = (m_eMove == Move_Mode::Rotate);
	if (ImGui::Selectable("E : Rotate", isRotate, ImGuiSelectableFlags_DontClosePopups,
		Btn))
		m_eMove = Move_Mode::Rotate;
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("������Ʈ�� ȸ����ŵ�ϴ�.");

	ImGui::End();
}

void CPickingMgr::MoveCamera(_float& dt)
{
	_float Move_speed = 5;
	_float Rotate_speed = 20;

	_long x = CInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X);
	_long y = CInputMgr::GetInstance()->Get_DIMouseMove(DIMD_Y);
	_long wheel = CInputMgr::GetInstance()->Get_DIMouseMove(DIMD_Z);

	CCamera* pCam = CCameraMgr::GetInstance()->Get_MainCamera();
	CTransform* pTrans = pCam->m_pOwner->Get_Component<CTransform>();

	_vec3 Look = pCam->Get_Dir();
	_vec3 up = pCam->Get_Up();
	_vec3 Right = pCam->Get_Right();

	if (m_eMove == Move_Mode::Rotate) {
			pCam->Add_Yaw(x * dt * Rotate_speed);
			pCam->Add_Pitch(y * dt * Rotate_speed);
	}
	else if (m_eMove == Move_Mode::Transform) {
		//x��ŭ ���������� //y��ŭ ����
		pTrans->Add_Pos(-Right * x * Move_speed * dt);
		pTrans->Add_Pos(up * y * Move_speed * dt);
		_vec3 MoveForward = Look * Move_speed * wheel * dt;
		pTrans->Add_Pos(MoveForward);
	}
	else if (m_eMove == Move_Mode::Scale) {

	}
}

void CPickingMgr::MoveObject(_float& dt)
{
	if (!m_pTarget)	return;
	_long x = CInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X);
	_long y = CInputMgr::GetInstance()->Get_DIMouseMove(DIMD_Y);
	_long wheel = CInputMgr::GetInstance()->Get_DIMouseMove(DIMD_Z);
	_float Rotate_speed = 80;
	_float Scale_speed = 10;

	CTransform* transform = m_pTarget->Get_Component<CTransform>();
	_vec3 objPos = transform->Get_Pos();
	_vec3 normal = objPos - m_tRay.vRayPos;

	if (m_eMove == Move_Mode::Transform) {
		//ī�޶�� ��ü�� ���� ���͸� ���ϰ�, �������� ����. -> �ٵ� �̰� {0,0,0} �ϰ� �佺���̽� ����� ���ص� �ǰڴ�.
		// �ٵ� �̰� vRayDir��(�ƴϳ�, ������ ī�޶� ������ ������ ���ؾ� �ϳ�)
		//�׸��� �� ��ü�� ��ġ�� ���� ���͸� �����Ͽ�, ����� �����.
		D3DXVec3Normalize(&normal, &normal);
		float d = -D3DXVec3Dot(&normal, &objPos); // ����: N �� P + d = 0

		//�׸��� �� ���� ����ĳ�����Ͽ�, ���� ��ǥ�� x,y,x�̵� ��.
		_vec3 rayPos = m_tRay.vRayPos;
		_vec3 rayDir = m_tRay.vRayDir;
		float denom = D3DXVec3Dot(&normal, &rayDir); //������ ����� �������� ������ Ȯ��

		if (fabs(denom) > 1e-6f) //������ ���� �������� Ȯ���ϴ� �ܰ�
		{
			float t = -(D3DXVec3Dot(&normal, &rayPos) + d) / denom;
			if (t >= 0.f)
			{
				_vec3 hitPos = rayPos + rayDir * t;	//������ ����� ������ ����
				transform->Set_Pos(hitPos);
			}
		}
		_vec3 MoveForward = normal * wheel * dt;
		transform->Add_Pos(MoveForward);
	}

	else if (m_eMove == Move_Mode::Rotate) {
		_vec3 Rotate = { y * Rotate_speed * dt,  x * Rotate_speed * dt,wheel * Rotate_speed * dt };
		transform->Add_Rotate(-Rotate);
	}
	else if (m_eMove == Move_Mode::Scale) {
		_vec3 Scale = { x * dt, -y * dt, wheel * dt };
		transform->Add_Scale(Scale* Scale_speed);
	}
}


void CPickingMgr::Free()
{
	Safe_Release(m_pDevice);
}
