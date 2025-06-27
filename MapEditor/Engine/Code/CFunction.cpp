#include "Engine_Define.h"
#include "CFunction.h"
#include <locale>
#include <codecvt>

string& CFunction::toString(const wstring& wstr)
{
    static string result; // 반환을 위한 static 변수 (주의: 쓰레드 안전 X)
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    result = conv.to_bytes(wstr);
    return result;
}
