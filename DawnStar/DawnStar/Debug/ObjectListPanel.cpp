#include <DawnStar/dspch.hpp>
#include "ObjectListPanel.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <cstring>

namespace DawnStar
{

	ObjectListPanel::ObjectListPanel()
	{
		_context = {};
	}

	void ObjectListPanel::OnImGuiRender()
	{

		// Show Object List
		ImGui::Begin("Entity hierachy");
		if(_context)
		{
			_context->_registry.each([&](auto entityID)
				{
					Entity entity{ entityID , _context.get() };
					if (entity && !entity.GetParent())
						DrawEntityNode(entity);
				});
		}
		ImGui::End();
		

		ImGui::Begin("Properties");
		if (_selectionEntity)
		{
			DrawComponents(_selectionEntity);
		}
		ImGui::End();
	}

	void ObjectListPanel::SetContext(Ref<Scene> &context)
	{
		_context = context;
	}

	const ImRect ObjectListPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		const auto& rc = entity.GetRelationship();
		
		ImGuiTreeNodeFlags flags = ((_selectionEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
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
			_selectionEntity = entity;
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
				Entity child = _context->GetEntity(childId);
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
			
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
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

		DrawComponent<TransformComponent>("Transform", entity, [entity](auto& component)
		{
			DawnStar::ImGuiUI::DrawVec3Control("Translation", component.Translation);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DawnStar::ImGuiUI::DrawVec3Control("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			DawnStar::ImGuiUI::DrawVec3Control("Scale", component.Scale, 1.0f);

			if(entity.HasComponent<UI::LayoutComponent>())
			{
				ImGui::Text("Controlled by UI::Layout");
			}
		});
		

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
		{
			ImGui::Checkbox("Enable", &component.Enable);
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));			

			float frameHeight = ImGui::GetFrameHeight();
			const float buttonSize = frameHeight * 3.0f;
			const float tooltipSize = frameHeight * 11.0f;
				
			uint64_t id = component.Texture == nullptr ? 0 : component.Texture->GetRendererID();
			ImGui::Text("Texture Id: %ld", id);
			ImGui::ImageButton(reinterpret_cast<ImTextureID>(id), { buttonSize, buttonSize }, { 1, 1 }, { 0, 0 }, 0);

			ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
		});

		DrawComponent<TextComponent>("Text", entity, [](auto& component)
		{
			ImGui::InputTextMultiline("Text", &component.TextString);
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));	
			ImGui::DragFloat("Kerning", &component.Kerning, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("LineSpacing", &component.LineSpacing, 0.1f, 0.0f, 100.0f);
		});
		

		DrawComponent<UI::SpriteRendererComponent>("UI Sprite Renderer", entity, [](auto& component)
		{
			ImGui::Checkbox("Enable", &component.Enable);
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));			

			float frameHeight = ImGui::GetFrameHeight();
			const float buttonSize = frameHeight * 3.0f;
			const float tooltipSize = frameHeight * 11.0f;
				
			uint64_t id = component.Texture == nullptr ? 0 : component.Texture->GetRendererID();
			ImGui::Text("Texture Id: %ld", id);
			ImGui::ImageButton(reinterpret_cast<ImTextureID>(id), { buttonSize, buttonSize }, { 1, 1 }, { 0, 0 }, 0);

			ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
		});

		DrawComponent<UI::LayoutComponent>("UI Layout", entity, [](auto& component)
		{
			DawnStar::ImGuiUI::DrawVec2Control("Anchor Min", component.AnchorMin,
											   {0.0f, 0.0f}, component.AnchorMax, 0.01f);
			DawnStar::ImGuiUI::DrawVec2Control("Anchor Max", component.AnchorMax,
											   component.AnchorMin, {1.0f, 1.0f}, 0.01f);
			DawnStar::ImGuiUI::DrawVec2Control("Pivot", component.Pivot, 
											   {0.0f, 0.0f}, {1.0f, 1.0f}, 0.01f);
			
			ImGui::DragFloat("Rotation", &component.Rotation, 0.1f, 0.0f, 0.0f);
			ImGui::Spacing();

			if(component.AnchorMin.x == component.AnchorMax.x)
			{	
				ImGui::DragFloat("X", &component.Box.x, 0.1f, 0.0f, 0.0f);
				ImGui::DragFloat("Width", &component.Box.z, 0.1f, 0.0f, 0.0f);
			}
			else
			{
				ImGui::DragFloat("Left", &component.Box.x, 0.1f, 0.0f, 0.0f);
				ImGui::DragFloat("Right", &component.Box.z, 0.1f, 0.0f, 0.0f);
			}							   
			if(component.AnchorMin.y == component.AnchorMax.y)
			{	
				ImGui::DragFloat("Y", &component.Box.y, 0.1f, 0.0f, 0.0f);
				ImGui::DragFloat("Height", &component.Box.w, 0.1f, 0.0f, 0.0f);
			}
			else
			{
				ImGui::DragFloat("Top", &component.Box.y, 0.1f, 0.0f, 0.0f);
				ImGui::DragFloat("Bottom", &component.Box.w, 0.1f, 0.0f, 0.0f);
			}
		});

		DrawComponent<UI::ButtonComponent>("UI Button", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Over color", glm::value_ptr(component.overColor));			
			ImGui::ColorEdit4("Press color", glm::value_ptr(component.pressColor));			
			ImGui::ColorEdit4("Normal color", glm::value_ptr(component.normalColor));	
			
			ImGui::Text("_over %d", component._over);
			ImGui::Text("_press %d", component._press);
			ImGui::Text("_release %d", component._release);		
		});


	}
}