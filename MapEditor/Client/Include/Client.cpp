﻿
#include "pch.h"
#include "Client.h"
#include "Engine_Define.h"

#include "CMainApp.h"
#include "CTimeMgr.h"
#include "CFrameMgr.h"
#include "GUISystem.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE g_HInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND    g_hWnd;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,    _In_ int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    SetProcessDPIAware();

    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(253);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));
    MSG msg;

    CMainApp* pMainApp = CMainApp::Create();
    if (nullptr == pMainApp)
        return FALSE;
    
    if (FAILED(CTimeMgr::GetInstance()->Ready_Timer(L"Timer_Immediate")))
        return FALSE;
    
    if (FAILED(CTimeMgr::GetInstance()->Ready_Timer(L"Timer_FPS")))
        return FALSE;
    
    if (FAILED(CFrameMgr::GetInstance()->Ready_Frame(L"Frame60", 60.f)))
        return FALSE;

    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {

            if (WM_QUIT == msg.message)
                break;

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {
            CTimeMgr::GetInstance()->Set_TimeDelta(L"Timer_Immediate");
            _float fTimerImmediate = CTimeMgr::GetInstance()->Get_TimeDelta(L"Timer_Immediate");
            
            if (CFrameMgr::GetInstance()->IsPermit_Call(L"Frame60", fTimerImmediate));
            {
                CTimeMgr::GetInstance()->Set_TimeDelta(L"Timer_FPS");
                _float fTimer_FPS = CTimeMgr::GetInstance()->Get_TimeDelta(L"Timer_FPS");
            
                pMainApp->Update_MainApp(fTimer_FPS);
                pMainApp->LateUpdate_MainApp(fTimer_FPS);
                pMainApp->Render_MainApp();
            }
        }
    }

    _ulong dwRefCnt(0);
    if (dwRefCnt = Engine::Safe_Release(pMainApp)) {
        //MSG_BOX("MainApp Release Failed");
        return FALSE;
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_HInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
    RECT rc{ 0, 0, WINCX, WINCY };

    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0,
        rc.right - rc.left,
        rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    g_hWnd = hWnd;

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Engine::GUISystem::GetInstance()->Set_Input(hWnd, message, wParam, lParam);

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(g_HInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            int result = MessageBoxW(
                g_hWnd,
                L"정말 종료하시겠습니까?",
                L"종료 확인",
                MB_YESNO | MB_ICONQUESTION
            );

            if (result == IDYES)
                DestroyWindow(g_hWnd);
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
