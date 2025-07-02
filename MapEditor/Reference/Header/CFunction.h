#pragma once
BEGIN(Engine)
class CGameObject;
class ENGINE_DLL CFunction
{
public:
	static string& toString(const wstring& string);
	static wstring toWString(const std::string& str);
};

END