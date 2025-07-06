#include "Engine_Define.h"
#include "CComponent.h"
#include "CGameObject.h"

HRESULT CComponent::Ready_Component()
{
	return E_NOTIMPL;
}

void CComponent::Update_Component(_float& dt)
{
}

void CComponent::LateUpdate_Component(_float& dt)
{
}

CComponent* CComponent::Clone() const
{
	return nullptr;
}

void CComponent::Delete_ComponentPane()
{
    if (ImGui::BeginPopupContextItem())
    {
        string popID = "Delete##" + Get_ComponentName();
        if (ImGui::MenuItem(popID.c_str()))
        {
            ImGui::CloseCurrentPopup(); //  �˾� �ݱ�
            m_bDelete = true;
        }
        ImGui::EndPopup(); //  ������ �� ���� ȣ��
    }
}
