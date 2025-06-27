#pragma once
#include "CBase.h"

BEGIN(Engine)
class CFrame;

class ENGINE_DLL CFrameMgr :
    public CBase
{
    DECLARE_SINGLETON(CFrameMgr)

private:
    explicit CFrameMgr();
    virtual ~CFrameMgr();

public:
    _bool IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta);
    HRESULT Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit);

private:
    CFrame* Find_Frame(const _tchar* pFrameTag);

private:
    map<const _tchar*, CFrame* > m_mapFrame;

private:
    virtual void Free();
};

END
