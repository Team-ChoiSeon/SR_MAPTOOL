#pragma once
#include "CGameObject.h"

BEGIN(Engine)
class CGameObject; // ���� ����

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
