#ifndef Engine_Define_h__
#define Engine_Define_h__

//--윈도우--//
#define NOMINMAX   // ← 반드시 windows.h 전에 정의해야 함
#include <windows.h>

//--다이렉트X--//
#include <d3d9.h>
#include <d3dx9.h>

//--STL--//
#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <queue>

#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <typeindex> //타입으로 해시
#include <process.h>//쓰레드

//--Define 헤더--//
#include "Engine_GUI.h"
#include "Engine_Enum.h"
#include "Engine_Function.h"
#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"

//---Define -- //
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma execution_character_set("utf-8")

#pragma warning(disable :4251) //맵 컨테이너 경고 무시

//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//
//#ifndef  DBG_NEW
//#define DBG_NEW new(_NORMAL_BLOCK , __FILE__ , __LINE__)
//#define new DBG_NEW
//
//#endif // ! DBG_NEW
//#endif // _DEBUG
#endif

using namespace std;
using namespace Engine;
