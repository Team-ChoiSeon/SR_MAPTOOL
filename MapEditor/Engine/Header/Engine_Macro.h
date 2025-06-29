#pragma once

namespace Engine {
#define WINCX 1600
#define WINCY 900


#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)	using namespace NAMESPACE;

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif

#define NO_COPY(CLASSNAME)																	\
	private : 																												\
		CLASSNAME(const CLASSNAME&) = delete;									\
		CLASSNAME& operator =(const CLASSNAME&) = delete;		

#define DECLARE_SINGLETON(CLASSNAME)	\
	NO_COPY(CLASSNAME)									\
private:																					\
	static CLASSNAME* m_pInstance;						\
public:																					\
	static CLASSNAME* GetInstance(void);			\
	static void DestroyInstance(void);						

	
#define NO_COPY(CLASSNAME)								\
		private:										\
		CLASSNAME(const CLASSNAME&) = delete;			\
		CLASSNAME& operator = (const CLASSNAME&)= delete;		

#define DECLARE_SINGLETON(CLASSNAME)					\
		NO_COPY(CLASSNAME)								\
		private:										\
		static CLASSNAME*	m_pInstance;				\
		public:											\
		static CLASSNAME*	GetInstance( void );		\
		static void DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)					\
	CLASSNAME* CLASSNAME::m_pInstance = nullptr;		\
	CLASSNAME* CLASSNAME::GetInstance(void)				   \
	{																											   \
		if (nullptr == m_pInstance) {													 \
			m_pInstance = new CLASSNAME;								   \
		}																										   \
		return m_pInstance;																	  \
	}																											   \
																													\
	void CLASSNAME::DestroyInstance(void)							\
	{																											   \
		if (nullptr != m_pInstance) {													  \
			delete m_pInstance;																 \
			m_pInstance = nullptr;															\
		}																											  \
	}		
}
