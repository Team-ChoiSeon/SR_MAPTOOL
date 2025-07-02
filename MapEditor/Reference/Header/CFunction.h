#pragma once
BEGIN(Engine)
class CFunction
{
public:
	static string& toString(const wstring& string);

	static wstring toWString(const std::string& str);

};

END