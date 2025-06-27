#pragma once
#include "CBase.h"

BEGIN(Engine)
class CTimer;

class ENGINE_DLL CTimeMgr :
    public CBase
{
    DECLARE_SINGLETON(CTimeMgr)

private:
    explicit CTimeMgr();
    virtual  ~CTimeMgr();

public:
    _float Get_TimeDelta(const _tchar* pTimerTag);
    void Set_TimeDelta(const _tchar* pTimerTag);
public:
    HRESULT Ready_Timer(const _tchar* pTimerTag);
private:
    CTimer* Find_Timer(const _tchar* pTimerTag);
private:
    unordered_map<const _tchar*, CTimer*> m_mapTimer;
private:
    virtual void Free() override;
};
END
