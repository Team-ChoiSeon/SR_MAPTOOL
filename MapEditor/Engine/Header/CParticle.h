#pragma once
#include "CComponent.h"

BEGIN(Engine)

struct Particle
{
    _vec3 vPos;
    _vec3 vVelocity;
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

private:
    void Set_Texture(const string& key);
private:
    int m_iTextureIndex = 0;
    //��ƼŬ ������Ʈ ����
    vector<Particle> m_vecParticles;     // ��ƼŬ ����Ʈ
    float m_fSpawnInterval = 2.15f;       // ���� �ֱ�
    float m_fElapsedTime = 0.f;       
    int m_iMaxParticles = 100;            // �ִ� ��ƼŬ ��
    int m_iPrevMaxParticles = 100;            // �ִ� ��ƼŬ ��

    //��ƼŬ ���� ����
    float m_fLifeTime = 3.5f;
    float m_fSize = 1.5f;
    _vec3 m_vVelocity = {0.f,0.f,0.f};
    _vec3 m_vPos = { 0.f,0.f,0.f };
    _vec3 m_vOffset = { 0.f,0.f,0.f };
    D3DCOLOR m_BaseColor = D3DCOLOR_ARGB(255, 255, 160, 0); // ���� ��Ȳ��
    LPDIRECT3DVERTEXBUFFER9 m_pVB = nullptr;
    CTexture* m_pTexture = nullptr;
private:
    void Free() override;
};

END