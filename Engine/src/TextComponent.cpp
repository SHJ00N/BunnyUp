#include "pch.h"
#include "TextComponent.h"
#include "Scene.h"
#include "ImGuiClass.h"

namespace Engine
{
	void TextComponent::OnAwake()
	{
		ownerGameObject->scene->RegistText(this);
	}

	void TextComponent::OnDestroy()
	{
		ownerGameObject->scene->UnregistText(this);
	}

	void TextComponent::OnImGui()
	{
		ImGui::InputText("Text", &m_text);
		ImGui::DragFloat4("Color", &m_color[0], 0.01f, 0.0f, 1.0f);
	}
}