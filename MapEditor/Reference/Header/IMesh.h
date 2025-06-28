#pragma once
#include "CBase.h"
BEGIN(Engine)
class ENGINE_DLL IMesh :
    public CBase
{
protected:
    explicit IMesh() = default;
    virtual ~IMesh() = default;
public:
    virtual HRESULT Ready_Mesh() PURE;
   virtual DWORD GetFVF() const PURE;
   virtual const LPDIRECT3DVERTEXBUFFER9& Get_VertexBuffer() PURE;
   virtual const LPDIRECT3DINDEXBUFFER9& Get_IndexBuffer() PURE;
};

END