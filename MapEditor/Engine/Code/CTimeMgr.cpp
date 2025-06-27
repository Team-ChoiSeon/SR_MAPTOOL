#include "Engine_Define.h"
#include "CTimeMgr.h"
#include "CTimer.h"

IMPLEMENT_SINGLETON(CTimeMgr)

CTimeMgr::CTimeMgr()
{
}

CTimeMgr::~CTimeMgr()
{
	Free();
}

_float CTimeMgr::Get_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	
	if (pTimer == nullptr)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

void CTimeMgr::Set_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	
	if (pTimer == nullptr)
		return;

	pTimer->Update_Timer();
}

HRESULT CTimeMgr::Ready_Timer(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();

	if (pTimer == nullptr)
		return E_FAIL;

	m_mapTimer.insert({ pTimerTag,pTimer });

	return S_OK;
}

CTimer* CTimeMgr::Find_Timer(const _tchar* pTimerTag)
{
	auto	iter = find_if(m_mapTimer.begin(),
		m_mapTimer.end(),
		[&pTimerTag](auto& pair)->bool {
			return !lstrcmpW(pTimerTag, pair.first);
		});

	if (iter == m_mapTimer.end())
		return nullptr;

	return iter->second;
}

void CTimeMgr::Free()
{
	for_each(m_mapTimer.begin(), m_mapTimer.end(),
		[](auto& pair)->void{
		_ulong dwRefCount = 0;
		dwRefCount = pair.second->Release();
		if (dwRefCount == 0)
			pair.second = nullptr;
	});

	m_mapTimer.clear();
}
