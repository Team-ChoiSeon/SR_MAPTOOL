#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"
#include "json.hpp"
extern HWND g_hWnd;
extern HINSTANCE g_HInst;

#include <d3d9.h>
#include <d3dx9.h>
#include<vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm> 
using namespace std;
#include "imgui.h"
#include "imgui_internal.h" // (선택사항)

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW
#define DBG_NEW new(_NORMAL_BLOCK , __FILE__ , __LINE__)
#define new DBG_NEW
#endif // !DBG_NEW


#endif // _DEBUG
#endif //PCH_H
