#include "Engine_Define.h"
#include "CFactoryMgr.h"

void CFactoryMgr::Register(const std::string& className, CreatorFunc creator)
{
    GetMap()[className] = creator;
}

CGameObject* CFactoryMgr::Create(const std::string& className)
{
    auto it = GetMap().find(className);
    if (it != GetMap().end())
        return it->second(); // 함수 호출
    return nullptr;
}

std::unordered_map<std::string, CreatorFunc>& CFactoryMgr::GetMap()
{
    static std::unordered_map<std::string, CreatorFunc> map;
    return map;
}
