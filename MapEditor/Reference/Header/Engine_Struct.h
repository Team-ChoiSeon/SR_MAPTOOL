#pragma once
#include "Engine_Typedef.h"
#include <string>
#include <functional>
namespace Engine {

	typedef struct tagVertexColor
	{
		_vec3 vPosition;
		_ulong dwColor;

	}VTXCOL;

	const _ulong	FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

	typedef struct tagVertexParticle
	{
		_vec3 vPosition;
		_ulong dwColor;
		_vec2 vTexUV;
	}VTXPARTICLE;

	const _ulong	FVF_PARTICLE = D3DFVF_XYZ| D3DFVF_DIFFUSE | D3DFVF_TEX1;

	typedef struct tagVertexTexture {
		_vec3 vPosition;
		_vec2 vTexUV;
	}VTXTEX;
	const _ulong	FVF_TEX = D3DFVF_XYZ | D3DFVF_TEX1;


	typedef struct tagVertexCubeTexture
	{
		_vec3		vPosition;
		_vec3		vTexUV;

	}VTXCUBE;

	const _ulong	FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0); // 텍스처의 UV 좌표 값을 FLOAT형 3개로 표현하겠다는 매크로(괄호안의 숫자 0의 의미는 본래 버텍스에 텍스쳐 UV값이 여러개가 올 수 있는데 그중 0번째 값을 지정하겠다는 의미)

	typedef struct tagVertexTile
	{
		_vec3		vPosition;
		_vec3		vNormal;
		_vec2		vTexUV;
		_vec3 tangent;		  // 이거 두개는 FVF 지원하지 않음.
		_vec3 binormal;	  // 이거 두개는 FVF 지원하지 않음.

	}VTXTILE;

	typedef struct tagIndex16
	{
		_ushort  _0;
		_ushort  _1;
		_ushort  _2;

	}INDEX16;

	typedef struct tagIndex32
	{
		_ulong	_0;
		_ulong	_1;
		_ulong	_2;

	}INDEX32;

	struct PANEL
	{
		bool bOpen = true;
		std::function<void()> callback;

		PANEL() = default;
		PANEL(bool open, std::function<void()> cb)
			: bOpen(open), callback(std::move(cb)) {
		}
	};

	
	typedef struct  tagRay
	{
		_vec3 vRayPos;
		_vec3 vRayDir;
	}RAY;

	typedef struct  tagAABB
	{
		_vec3 vMin;
		_vec3 vMax;
	}AABB;

	class CGameObject;
	typedef struct tagRayHit {
		CGameObject* target;
		float fDistance;
	}RAYHIT;


}