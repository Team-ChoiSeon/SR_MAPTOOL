#include "Engine_Define.h"
#include "CInputMgr.h"

IMPLEMENT_SINGLETON(CInputMgr)

CInputMgr::CInputMgr()
{
}

CInputMgr::~CInputMgr()
{
}

HRESULT CInputMgr::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{

	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL)))
		return E_FAIL;

	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;


	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();


	return S_OK;
}

void CInputMgr::Update_InputDev()
{
	memcpy(m_PrevByKeyState, m_CurByKeyState, sizeof(BYTE) * 256); //이전 프레임 값 복사
	m_pKeyBoard->GetDeviceState(256, m_CurByKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

void CInputMgr::LateUpdate_InputDev(void)
{
}

_long CInputMgr::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	switch (eMouseState)
	{
	case Engine::DIMS_X:
		return m_tMouseState.lX;

	case Engine::DIMD_Y:
		return m_tMouseState.lY;

	case Engine::DIMD_Z: // 마우스 휠 이동량  이게 휠!
		return m_tMouseState.lZ;

	case Engine::DIMS_END:
		return 0;

	default:
		return 0;
	}
}

_byte CInputMgr::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	switch (eMouse)
	{
	case Engine::DIM_LB:
		return m_tMouseState.rgbButtons[0];
	case Engine::DIM_RB:
		return m_tMouseState.rgbButtons[1];
	case Engine::DIM_MB:
		return m_tMouseState.rgbButtons[2];
	case Engine::DIM_END:
		return 0;
	default:
		return 0;

	}
}

void CInputMgr::Free()
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}