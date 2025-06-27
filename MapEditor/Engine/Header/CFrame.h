#pragma once
#include "CBase.h"
BEGIN(Engine)
class ENGINE_DLL CFrame :
    public CBase
{
private:
    explicit CFrame();
    virtual ~CFrame();

public:
    _bool isPermit_Call(const _float& fTimeDelta);

public:
    HRESULT Ready_Frame(const _float& fCallLimit);

private:
    _float m_fCallLimit;
    _float m_fAccTimeDelta;

public:
    static CFrame* Create(const _float& fCallLimit);

private:
    virtual void Free() override;
};
END
