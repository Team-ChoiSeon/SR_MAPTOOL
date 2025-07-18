#pragma once

class _declspec(dllexport) CBase 
{
protected:
	inline explicit CBase();
	inline virtual ~CBase();

public:
	inline unsigned long AddRef();
	inline unsigned long Release();

private:
	unsigned long m_dwRefCount;
public:
	inline virtual void Free() = 0;
};

#include "CBase.inl"