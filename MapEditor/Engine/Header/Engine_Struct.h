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

	const _ulong	FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0); // �ؽ�ó�� UV ��ǥ ���� FLOAT�� 3���� ǥ���ϰڴٴ� ��ũ��(��ȣ���� ���� 0�� �ǹ̴� ���� ���ؽ��� �ؽ��� UV���� �������� �� �� �ִµ� ���� 0��° ���� �����ϰڴٴ� �ǹ�)

	typedef struct tagVertexTile
	{
		_vec3		vPosition;
		_vec3		vNormal;
		_vec2		vTexUV;
		_vec3 tangent;		  // �̰� �ΰ��� FVF �������� ����.
		_vec3 binormal;	  // �̰� �ΰ��� FVF �������� ����.

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