#pragma once
#include "IMesh.h"

BEGIN(Engine)
class ENGINE_DLL CCubeMesh :
    public IMesh
{
private:
    explicit CCubeMesh();
    virtual ~CCubeMesh();
public:
    static CCubeMesh* Create();
public:
    HRESULT Ready_Mesh() override;
    const LPDIRECT3DVERTEXBUFFER9& Get_VertexBuffer() override { return m_pVB; };
    const LPDIRECT3DINDEXBUFFER9& Get_IndexBuffer() override { return m_pIB; };
    DWORD GetFVF() const override;

private:
    LPDIRECT3DVERTEXBUFFER9 m_pVB;
    LPDIRECT3DINDEXBUFFER9 m_pIB;
    LPDIRECT3DDEVICE9 m_pDevice;
private:
    void Free() override;
};
END
