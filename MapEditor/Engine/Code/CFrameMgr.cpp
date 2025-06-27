#include "Engine_Define.h"
#include "CFrameMgr.h"
#include "CFrame.h"

IMPLEMENT_SINGLETON(CFrameMgr)

CFrameMgr::CFrameMgr()
{
}

CFrameMgr::~CFrameMgr()
{
	Free();
}

_bool CFrameMgr::IsPermit_Call(const _tchar* pFrameTag,const _float& fTimeDelta)
{
	CFrame* pFrame = Find_Frame(pFrameTag);

	if (nullptr == pFrame)
		return false;

	return pFrame->isPermit_Call(fTimeDelta);
}

HRESULT CFrameMgr::Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit)
{
	CFrame* pFrame = Find_Frame(pFrameTag);
	
	if (pFrame != nullptr)
		return E_FAIL;
	
	pFrame = CFrame::Create(fCallLimit);

	if (pFrame == nullptr)
		return E_FAIL;

	m_mapFrame.insert({ pFrameTag,pFrame });

	return S_OK;
}

CFrame* CFrameMgr::Find_Frame(const _tchar* pFrameTag)
{
	auto iter = find_if(m_mapFrame.begin(), m_mapFrame.end(),
		[&pFrameTag](auto& pair)->bool {
			return !(lstrcmpW(pFrameTag, pair.first));
		});

	if (iter == m_mapFrame.end())
		return nullptr;

	return iter->second;
}

void CFrameMgr::Free()
{
	for_each(m_mapFrame.begin(), m_mapFrame.end(),
		[](auto& pair)->void {
			_ulong dwRefCnt = 0;
			dwRefCnt = pair.second->Release();
			if (dwRefCnt == 0) {
				pair.second = nullptr;
			}
		});

	m_mapFrame.clear();
}
