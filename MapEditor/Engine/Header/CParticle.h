#pragma once
#include "CComponent.h"

BEGIN(Engine)
enum PARTICLE_MOVE_TYPE
{ FIRE, DUST,BREATH,RADIAL,SPREAD};

enum PARTICLE_STATE
{
    EMITTING, STOPPING, NONEACTIVE,
};

struct Particle
{
    _vec3 vPos;
    _vec3 vVelocity;
    PARTICLE_MOVE_TYPE moveType;
    float fLifeTime;
    float fAge;
    float fSize;
    D3DCOLOR color;
    bool bActive;
};

class CTexture;


class ENGINE_DLL CParticle :
    public CComponent
{
private:
    explicit CParticle();
    virtual ~CParticle();
public:
    static CParticle* Create();
public:
    HRESULT Ready_Component() override;
    void Update_Component(_float& dt) override;
    void LateUpdate_Component(_float& dt) override;
    void Render_Particle(LPDIRECT3DDEVICE9 pDevice);
    void Render_Panel(ImVec2 size) override;

    void Emit_Particle();
    CComponent* Clone() const override;
    void Serialize(json& outJson) const override;
    void Deserialize(const json& inJson) override;
    string Get_ComponentName() const override;

    void Request_Emit();
private:
    void Set_Texture(const string& key);
    float randRange(float min, float max);
    int Particle_Count();

private:
    int m_iTextureIndex = 0;

    //��ƼŬ ������Ʈ ���� (�� ��ƼŬ���� ���� ���� �� ��)
    vector<Particle> m_vecParticles;     // ��ƼŬ ����Ʈ
    float m_fSpawnInterval = 0.5f;       // ���� �ֱ�
    float m_fElapsedTime = 0.f;       
    int m_iMaxParticles = 100;            // �ִ� ��ƼŬ ��
    int m_iPrevMaxParticles = 100;            // �ִ� ��ƼŬ ��

    //�վ �ð��� Ʈ����
    _bool m_bLoop;
    float m_fEmitLife = 3.5f;
    float m_fElapsedEmit = 0.f;

    //��ƼŬ ���� ����
    float m_fLifeTime = 3.5f;
    float m_fSize = 1.5f;
    
    _vec3 m_vVelocity = {0.f,0.f,0.f};
    _vec3 m_vPos = { 0.f,0.f,0.f };
    _vec3 m_vOffset = { 0.f,0.f,0.f };

    D3DCOLOR m_BaseColor = D3DCOLOR_ARGB(255, 255, 160, 100); // ���� ��Ȳ��
    LPDIRECT3DVERTEXBUFFER9 m_pVB = nullptr;

    int m_iMovementType = 0;
    PARTICLE_MOVE_TYPE m_eMoveType = DUST;
    PARTICLE_STATE m_eNowState = EMITTING;

    CTexture* m_pTexture = nullptr;

    int m_EmitCount = 0;
    float m_fAngle = 0;
    float m_fSpeed = 1.f;
private:
    void Free() override;
};

END