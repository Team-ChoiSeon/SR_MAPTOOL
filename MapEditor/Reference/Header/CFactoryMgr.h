#pragma once
#include "CGameObject.h"

BEGIN(Engine)
class CGameObject; // 전방 선언

using CreatorFunc = std::function<CGameObject* ()>;

class ENGINE_DLL CFactoryMgr
{
    public:
        static void Register(const std::string& className, CreatorFunc creator);
        static CGameObject* Create(const std::string& className);

    private:
        static std::unordered_map<std::string, CreatorFunc>& GetMap();
};

END
