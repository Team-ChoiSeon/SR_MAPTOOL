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
#include "IMesh.h"


IMPLEMENT_SINGLETON(CPickingMgr)

CPickingMgr::CPickingMgr()
	:m_pDevice(nullptr),m_pTarget(nullptr)
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
	ScreenToClient(m_hwnd, &pt);  // 스크린 → 클라이언트
	m_CursorPos = pt;
	if (GUISystem::GetInstance()->UsingUI()) return;

	ComputeRay();
	Start_RayCasting();
	if (m_pTarget)
	{
		if (!CInputMgr::GetInstance()->Mouse_Hold(DIM_LB)) return;
		//카메라와 물체의 방향 벡터를 구하고, 법선으로 지정. -> 근데 이건 {0,0,0} 하고 뷰스페이스 역행렬 곱해도 되겟다.
		// 근데 이게 vRayDir네(아니네, 중점과 카메라 중점의 방향을 구해야 하네)
		CTransform* transform = m_pTarget->Get_Component<CTransform>();
		//그리고 그 물체의 위치와 방향 벡터를 외적하여, 평면을 만들어.
		_vec3 objPos = transform->Get_Pos();
		_vec3 normal = objPos-m_tRay.vRayPos;
		D3DXVec3Normalize(&normal, &normal);
		float d = -D3DXVec3Dot(&normal, &objPos); // 평면식: N 돗 P + d = 0

		//그리고 그 평면과 레이캐스팅하여, 구한 좌표로 x,y,x이동 해.
		_vec3 rayPos = m_tRay.vRayPos;
		_vec3 rayDir = m_tRay.vRayDir;
		float denom = D3DXVec3Dot(&normal, &rayDir);

		if (fabs(denom) > 1e-6f)
		{
			float t = -(D3DXVec3Dot(&normal, &rayPos) + d) / denom;
			if (t >= 0.f)
			{
				_vec3 hitPos = rayPos + rayDir * t;
				transform->Set_Pos(hitPos);
			}
		}
	}
}

void CPickingMgr::LateUpdate_Picking(_float& dt)
{
}

void CPickingMgr::ComputeRay()
{
	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9)); //dx 뷰포트 가져오기
	m_pDevice->GetViewport(&ViewPort);

	_vec3 ndc;
	//NDC 좌표로 변환
	// 만약 x가 600이라면 (뷰포트 맨 왼쪽 - 마우스 포지션 / 뷰포트 넓이) 
	//이러면 0부터 1까지 사이에 어디에 위치한지가 나옴.
	//그때 2를 곱하면 0부터 2까지 사이에 어디에 위치한지가 나옴
	//이때 1을 빼면 -1~1사이에 위치한지가 나옴.
	ndc.x = ((float)m_CursorPos.x / (float)ViewPort.Width) * 2.0f - 1.0f;
	ndc.y = 1.0f - ((float)m_CursorPos.y / (float)ViewPort.Height) * 2.0f;
	ndc.z = 1.0f;

	// 투영 -> 뷰 스페이스
	D3DXMATRIX	matProj;
	matProj = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ProjMatrix();
	D3DXMatrixInverse(&matProj, 0, &matProj); //역행렬 돌려줌.
	D3DXVec3TransformCoord(&ndc, &ndc, &matProj); //그 역행렬을 기준으로 ndc를 계산함.
	//이러면 뷰 스페이스가 나오게됨 (즉 카메라 원점으로 돌린 그 공간)

	// 뷰 스페이스 -> 월드
	D3DXMATRIX	matView;
	matView = CCameraMgr::GetInstance()->Get_MainCamera()->Get_ViewMatrix();
	D3DXMatrixInverse(&matView, 0, &matView);

	m_tRay.vRayPos = { 0.f, 0.f, 0.f }; //이건 카메라의 위치다. 왜냐면, 카메라는 원점으로 돌리고 z1을 바라보니까(뷰스페이스 상황)
	//클릭을 하게 되면 카메라에서 나오는거니까-> 카메라 위치에서 광선이 나오게 되는 것이 맞다.
	m_tRay.vRayDir = ndc - m_tRay.vRayPos; //그렇기에 지금 카메라에서 ndc로 향하는 방향을 구한다

	D3DXVec3TransformCoord(&m_tRay.vRayPos, &m_tRay.vRayPos, &matView); //레이포즈를 월드 위치 벡터로 변환
	D3DXVec3TransformNormal(&m_tRay.vRayDir, &m_tRay.vRayDir, &matView); //레이 디렉션을 월드 방향 벡터로 변환
	D3DXVec3Normalize(&m_tRay.vRayDir, &m_tRay.vRayDir);

	GUISystem::GetInstance()->RegisterPanel("Debug", [this]() {
		ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Once); // ← 여기서 크기 설정
		ImGui::Begin("Debug", nullptr, 0);
		ImGui::Text("Cursor: %d, %d", m_CursorPos.x, m_CursorPos.y);
		ImGui::End();
		});
	//여기까지 우선 월드 스페이스 기준 레이 위치와 방향임.
}

RAY CPickingMgr::RayToLocal(CTransform* transform)
{
	RAY localRay;
	
	const D3DXMATRIX& worldMat = transform->Get_WorldMatrix(); //트랜스폼의 월드 매트릭스

	D3DXMATRIX invWorldMat;
	D3DXMatrixInverse(&invWorldMat, nullptr, &worldMat); // 역행렬 따로 저장

	D3DXVec3TransformCoord(&localRay.vRayPos, &m_tRay.vRayPos, &invWorldMat);  //레이포즈를 로컬 위치 벡터로 변환
	D3DXVec3TransformNormal(&localRay.vRayDir, &m_tRay.vRayDir, &invWorldMat); //레이 디렉션을 로컬 방향 벡터로 변환
	D3DXVec3Normalize(&localRay.vRayDir, &localRay.vRayDir); // 정규화 

	return localRay;
}

void CPickingMgr::Start_RayCasting()
{
	if (!CInputMgr::GetInstance()->Mouse_Down(DIM_LB))  return;

	CScene* nowScene = CSceneMgr::GetInstance()->Get_NowScene();

	_float distance = FLT_MAX; //거리 우선 순위 계산 
	CGameObject* pPickedObject = nullptr; //피킹된 순서대로 넣어줄 변수

	int max = static_cast<int>(LAYER_ID::L_END);
	for (int i = 0; i< max; ++i) {
		LAYER_ID id = static_cast<LAYER_ID>(i);
		CLayer* tmpLayer = nowScene->Get_Layer(id);
		if (!tmpLayer) continue;

		vector<CGameObject*>& objVector = tmpLayer->Get_Object();

		if (objVector.empty()) continue;

		for (CGameObject* obj : objVector) {
			_float nowDistance = Calc_ObjRay(obj);
			if (nowDistance < 0) continue; //교차하지 않으면 음수

			if (distance > nowDistance) {
				distance = nowDistance;
				pPickedObject = obj;
			}
		}
	}

	m_pTarget = pPickedObject;
}

_float CPickingMgr::Calc_ObjRay(CGameObject* obj)
{
	// 레이: 월드 → 로컬 변환
	CTransform* transform = obj->Get_Component<CTransform>();
	if (!transform) return -1.f;

	RAY localRay = RayToLocal(transform);

	CModel* model = obj->Get_Component<CModel>();
	if (!model) return -1.f;

	IMesh* mesh = model->Get_Mesh();
	AABB objBox = mesh->Get_AABBBOX();

	const D3DXVECTOR3& rayOriginLocal = localRay.vRayPos;
	const D3DXVECTOR3& rayDirLocal = localRay.vRayDir;

	const D3DXVECTOR3& localMin = objBox.vMin;
	const D3DXVECTOR3& localMax = objBox.vMax;

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
				return -1.f;											//충돌 안함 -> 왜냐면, 그 안에 있는 것도 아니면서 그 축을 벗어나지 않는다는 뜻이니까
		}
		else
		{
			_float  t1 = (minB - origin) / dir;				//광선 위치에서 min까지 (무슨 방향?? 양 or 음)얼마나 이동해야 하는지
			_float  t2 = (maxB - origin) / dir;				//광선 위치에서 max까지 (무슨 방향?? 양 or 음)얼마나 이동해야 하는지

			if (t1 > t2) swap(t1, t2);							//둘중 작은 거 큰거 바꾸고

			tmin = max(tmin, t1);	// 들어가는 시점 중 가장 늦은 순간(무한이랑 비교해야 가장 늦은 것을 할 수 있음)
			tmax = min(tmax, t2); // 나오는 시점 중 가장 빠른 순간(무한이랑 비교해야 가장 빠른 것을 할 수 있음)

			
			if (tmin > tmax)
				return -1.f;
		}
	}

	// 광선이 박스 뒤에 있음
	if (tmax < 0)
		return -1.f;

	// 교차 거리 반환
	return tmin;
}

void CPickingMgr::Free()
{
	Safe_Release(m_pDevice);
}
