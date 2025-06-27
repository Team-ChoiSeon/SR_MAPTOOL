#pragma once
namespace Engine {
	enum WINMODE { MODE_FULL, MODE_WIN };
	
	enum COMPONENTID {ID_DYNAMIC, ID_STATIC, ID_END};
	
	enum INFO {INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS,INFO_END};
	enum ROTATION {ROT_X,ROT_Y,ROT_Z, ROT_END};

	enum TEXTUREID{TEX_NORMAL, TEX_CUBE, TEX_END};
	enum RENDERID {RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI,RENDER_END};
	
	enum MOUSEKEYSTATE{DIM_LB,DIM_RB,DIM_MB,DIM_END};
	enum MOUSEMOVESTATE{DIMS_X,DIMD_Y,DIMD_Z,DIMS_END};
	
	enum class RENDER_PASS
	{
		RP_SKYBOX,        // // 스카이박스 (Z-write 꺼짐, 가장 먼저)
		RP_SHADOW,        // 그림자 맵을 생성하는 패스 (예: Depth-Only 렌더링 → ShadowMap 생성용)
		RP_OPAQUE,        // 불투명 오브젝트 렌더링 (Z-Buffer 사용, 일반적인 Mesh들)
		RP_STENCIL,   // 반투명 오브젝트 렌더링 (Z-Buffer 정렬 필요, 알파 블렌딩 사용)
		RP_TRANSPARENT,   // 반투명 오브젝트 렌더링 (Z-Buffer 정렬 필요, 알파 블렌딩 사용)
		RP_UI,            // 2D UI 요소 렌더링 (HUD, 텍스트 등, 보통 정해진 순서대로 출력)
		RP_POSTPROCESS,   // 후처리 단계 (Bloom, Blur, Color Correction 등 → FullScreen Quad 기반)
		RP_DEBUG,   // 디버그용 출력 표시들
		RP_END            // enum 끝 표시 (배열 크기 지정 등 루프 용도)
	};

}