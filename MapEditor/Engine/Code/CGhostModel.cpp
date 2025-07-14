#include "Engine_Define.h"
#include "CGhostModel.h"
#include "CGraphicDev.h"
#include "CRenderMgr.h"
#include "CGameObject.h"
#include "CTransform.h"

CGhostModel::CGhostModel()
{
}

CGhostModel::~CGhostModel()
{
}

CGhostModel* CGhostModel::Create()
{
	CGhostModel* instance = new CGhostModel;

	if (FAILED(instance->Ready_Component())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

HRESULT CGhostModel::Ready_Component()
{
	m_pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

	vector<VTXCOL>    vertices;
	_ulong col =(_ulong)D3DCOLOR_ARGB(255, 255, 0, 255);
	vertices.push_back({ {-1,+1,-1},col });		//0
	vertices.push_back({ {+1,+1,-1},col });		//1
	vertices.push_back({ {+1,-1,-1},col });		//2
	vertices.push_back({ {-1,-1,-1},col });		//3

	vertices.push_back({ {-1,+1,+1},col });		//4
	vertices.push_back({ {+1,+1,+1},col });		//5
	vertices.push_back({ {+1,-1,+1},col});		//6
	vertices.push_back({ {-1,-1,+1},col });		//7

	if (FAILED(m_pDevice->CreateVertexBuffer(
		vertices.size() * sizeof(VTXTILE),
		0, FVF_COL, D3DPOOL_MANAGED, &m_pVB, 0))) {
		return E_FAIL;
	}

	void* pVBData = nullptr;
	m_pVB->Lock(0, 0, &pVBData, 0);
	memcpy(pVBData, vertices.data(), vertices.size() * sizeof(VTXTILE));
	m_pVB->Unlock();

	vector<DWORD> indices;
	indices.resize(36);
	//front
	indices[0] = 3;	indices[1] = 0;	indices[2] = 1;
	indices[3] = 3;	indices[4] = 1;	indices[5] = 2;
	//back
	indices[6] = 6;	indices[7] = 5;	indices[8] =4;
	indices[9] = 6;	indices[10] = 4;	indices[11] = 7;
	//right
	indices[12] = 2;	indices[13] = 1;	indices[14] = 5;
	indices[15] = 2;	indices[16] = 5;	indices[17] = 6;
	//left
	indices[18] = 7;	indices[19] = 4;	indices[20] =0;
	indices[21] = 7;	indices[22] = 0;	indices[23] = 3;
	//up
	indices[24] = 0;	indices[25] = 4;	indices[26] = 5;
	indices[27] = 0;	indices[28] = 5;	indices[29] = 1;
	//down
	indices[30] = 7;	indices[31] = 3;	indices[32] = 2;
	indices[33] = 7;	indices[34] = 2;	indices[35] = 6;

	if (FAILED(m_pDevice->CreateIndexBuffer(
		indices.size() * sizeof(DWORD), 0, 
		D3DFMT_INDEX32, D3DPOOL_MANAGED ,&m_pIB, 0))){
			return E_FAIL;
		}

	void* pIBDate = nullptr;
	m_pIB->Lock(0, 0, &pIBDate, 0);
	memcpy(pIBDate, indices.data(), indices.size() * sizeof(DWORD));
	m_pIB->Unlock();

	return S_OK;
}

void CGhostModel::Update_Component(_float& dt)
{
}

void CGhostModel::LateUpdate_Component(_float& dt)
{
	CRenderMgr::GetInstance()->Add_GhostRenderer(this);
}

void CGhostModel::Render(LPDIRECT3DDEVICE9 pDevice)
{
	m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pDevice->SetTransform(D3DTS_WORLD, 
		&m_pOwner->Get_Component<CTransform>()->Get_WorldMatrix());

	pDevice->SetFVF(FVF_COL);
	pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXCOL));
	pDevice->SetIndices(m_pIB);
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
	m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

}

void CGhostModel::Render_Panel(ImVec2 size)
{
}

CComponent* CGhostModel::Clone() const
{
	return nullptr;
}

void CGhostModel::Serialize(json& outJson) const
{
}

void CGhostModel::Deserialize(const json& inJson)
{
}

string CGhostModel::Get_ComponentName() const
{
	return "CGhostModel";
}

void CGhostModel::Free()
{
}
