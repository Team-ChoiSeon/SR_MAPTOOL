#pragma once
#include "CBase.h"
BEGIN(Engine)

class ENGINE_DLL CMesh :
    public CBase
{
private:
   explicit CMesh();
   virtual  ~CMesh();
public:
    static CMesh* Create();

public:
    HRESULT LoadOBJ(LPDIRECT3DDEVICE9 pDevice,const std::string& filePath);
    void Render(LPDIRECT3DDEVICE9 pDevice);

    string& Get_Key() { return m_strKey; }
    void Set_Key(const string& key) {  m_strKey = key; }
private:
    HRESULT Ready_Mesh();

private:
    LPDIRECT3DVERTEXBUFFER9 m_pVB = nullptr;
    LPDIRECT3DINDEXBUFFER9  m_pIB = nullptr;

    DWORD m_dwFVF = 0;
    _uint m_iPrimitiveCount = 0;
    _uint m_iVtxCount = 0;
    _uint m_iVertexStride = 0;
    LPDIRECT3DVERTEXDECLARATION9 g_pDecl = nullptr;
   string m_strKey;

private:
    void Free() override;
};

END