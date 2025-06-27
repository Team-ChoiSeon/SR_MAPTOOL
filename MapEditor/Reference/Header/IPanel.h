#pragma once
#include "CBase.h"

BEGIN(Engine)
class CScene;

  class ENGINE_DLL  IPanel :
    public CBase
{
public:
    IPanel() = default;
    virtual ~IPanel() = default;

public:	
    virtual HRESULT Ready_Panel() PURE;
    virtual void Update_Panel(_float& dt) PURE;
    virtual void LateUpdate_Panel(_float& dt) PURE;
    virtual void Render_Panel() PURE;

public:
    CScene* m_pOwner;
};

END