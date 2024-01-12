#include <DawnStar/dspch.hpp>
#include "ObjectListPanel.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <filesystem>

namespace DawnStar
{

	ObjectListPanel::ObjectListPanel()
	{
		m_Context = {};
	}

	void ObjectListPanel::OnImGuiRender()
	{

		// Show Object List
		ImGui::Begin("Entity hierachy");
		if(m_Context)
		{
			m_Context->m_Registry.each([&](auto entityID)
				{
					Entity entity{ entityID , m_Context.get() };
					if (entity && !entity.GetParent())
						DrawEntityNode(entity);
				});
		}
		ImGui::End();
		

		ImGui::Begin("Properties");
		if (m_SelectionEntity)
		{
			DrawComponents(m_SelectionEntity);
		}
		ImGui::End();
	}

	void ObjectListPanel::SetContext(Ref<Scene> &context)
	{
		m_Context = context;
	}

	const ImRect ObjectListPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		const auto& rc = entity.GetRelationship();
		
		ImGuiTreeNodeFlags flags = ((m_SelectionEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_FramePadding;		
		if (rc.Children.size() == 0)
		{
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionEntity = entity;
		}

		ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
		verticalLineStart.x -= 10.0f;
		verticalLineStart.y -= ImGui::GetFrameHeight() * 0.5f;
		const ImRect nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		const ImColor treeLineColor = ImColor(159, 162, 246);

		if (opened)
		{		
			ImDrawList* drawList = ImGui::GetWindowDrawList();

			ImVec2 verticalLineEnd = verticalLineStart;
			constexpr float lineThickness = 1.5f;

			for (const auto& childId : rc.Children)
			{
				Entity child = m_Context->GetEntity(childId);
				const float HorizontalTreeLineSize = child.GetRelationship().Children.empty() ? 18.0f : 9.0f; //chosen arbitrarily
				const ImRect childRect = DrawEntityNode(child);

				const float midpoint = (childRect.Min.y + childRect.Max.y) / 2.0f;
				drawList->AddLine(ImVec2(verticalLineStart.x, midpoint), ImVec2(verticalLineStart.x + HorizontalTreeLineSize, midpoint), treeLineColor, lineThickness);
				verticalLineEnd.y = midpoint;
			}

			drawList->AddLine(verticalLineStart, verticalLineEnd, treeLineColor, lineThickness);

			if (opened && rc.Children.size() > 0)
				ImGui::TreePop();
		}
		return nodeRect;
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name.c_str());
			ImGui::PopStyleVar();

			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void ObjectListPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			// char buffer[256];
			// memset(buffer, 0, sizeof(buffer));
			// strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
			// if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			// {
			// 	tag = std::string(buffer);
			// }
		}

		ImGui::SameLine();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			DawnStar::ImGuiUI::DrawVec3Control("Translation", component.Translation);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DawnStar::ImGuiUI::DrawVec3Control("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			DawnStar::ImGuiUI::DrawVec3Control("Scale", component.Scale, 1.0f);
		});
		

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));			

			float frameHeight = ImGui::GetFrameHeight();
			const float buttonSize = frameHeight * 3.0f;
			const float tooltipSize = frameHeight * 11.0f;
				
			uint64_t id = component.Texture == nullptr ? 0 : component.Texture->GetRendererID();
			ImGui::Text("Texture Id: %ld", id);
			ImGui::ImageButton(reinterpret_cast<ImTextureID>(id), { buttonSize, buttonSize }, { 1, 1 }, { 0, 0 }, 0);

			ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
		});
		

		DrawComponent<UI::SpriteRendererComponent>("UI Sprite Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));			

			float frameHeight = ImGui::GetFrameHeight();
			const float buttonSize = frameHeight * 3.0f;
			const float tooltipSize = frameHeight * 11.0f;
				
			uint64_t id = component.Texture == nullptr ? 0 : component.Texture->GetRendererID();
			ImGui::Text("Texture Id: %ld", id);
			ImGui::ImageButton(reinterpret_cast<ImTextureID>(id), { buttonSize, buttonSize }, { 1, 1 }, { 0, 0 }, 0);

			ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
		});



	}
}