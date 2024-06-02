#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <DawnStar/Core/UUID.hpp>
#include <DawnStar/Core/Core.hpp>

#include <DawnStar/Renderer/Texture.hpp>
#include <DawnStar/Renderer/Camera.hpp>
#include <DawnStar/Renderer/Font.hpp>
#include <DawnStar/Renderer/Mesh.hpp>
#include <DawnStar/Renderer/FrameBuffer.hpp>

namespace DawnStar
{
	struct BaseComponent
	{
		bool Enable = true;
	};

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const uint64_t id)
			: ID(id) {}
		operator uint64_t() { return ID; }
	};

	struct TagComponent
	{
		std::string Tag;
		uint16_t Layer = BIT(1);
		bool Enabled = true;

		bool handled = true;

		TagComponent() = default;
		explicit TagComponent(const std::string& tag)
			: Tag(tag) {}
	};
    
	class Entity;
	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	};
	
	struct RelationshipComponent
	{
		UUID Parent = 0;
		std::vector<UUID> Children;
	};

	struct SpriteRendererComponent : public BaseComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture = nullptr;
		int32_t SortingOrder = 0;
		float TilingFactor = 1.0f;
	};

	struct CameraComponent : public BaseComponent
	{
		Camera Cam;
		bool Primary = true;
		bool FixedAspectRatio = false;
	};

	struct PrefabComponent
	{
		UUID ID;
	};
	
	struct TextComponent : public BaseComponent
	{
		std::string TextString;
		Ref<Font> FontAsset = Font::GetDefault();
		glm::vec4 Color{ 1.0f };
		float Kerning = 0.0f;
		float LineSpacing = 0.0f;
		float FontSize = 1.0f;

		// 0000 0000
		// [][] [bottom][centor][top] [right][centor][left]
		uint8_t Align = 0b0000'1001; 

		bool _updated = false; // TODO: 
		float width, height; // TODO: TextSystem을 만들어서 TextString이 변경될 때마다 update

		void SetHorizontalLeft()
		{	
			Align = (Align & 0b0011'1000) | 0b0000'0001; 
		}
		void SetHorizontalCenter()
		{	
			Align = (Align & 0b0011'1000) | 0b0000'0010; 
		}
		void SetHorizontalRight()
		{	
			Align = (Align & 0b0011'1000) | 0b0000'0100; 
		}
		void SetVerticalTop()
		{	
			Align = (Align & 0b0000'0111) | 0b0000'1000; 
		}
		void SetVerticalCenter()
		{	
			Align = (Align & 0b0000'0111) | 0b0001'0000; 
		}
		void SetVerticalBottom()
		{	
			Align = (Align & 0b0000'0111) | 0b0010'0000; 
		}


	};

	///////////////////////////////////
	///////     3D              ///////
	///////////////////////////////////
	struct MeshComponent
	{
		enum class CullModeType {Unknown = -1, Front, Back, DoubleSided};

		Ref<Mesh> MeshGeometry = nullptr;
		size_t SubmeshIndex = 0;
		CullModeType CullMode = CullModeType::Back;
	};

	///////////////////////////////////
	///////     Light           ///////
	///////////////////////////////////
	struct SkyLightComponent
	{
		Ref<TextureCubemap> Texture = nullptr;
		float Intensity = 0.7f;
		float Rotation = 0.0f;
	};

	struct LightComponent
	{
		enum class LightType { Directional = 0, Point, Spot };
		enum class ShadowQualityType { Hard = 0, Soft, UltraSoft };

		LightType Type = LightType::Point;
		bool UseColorTemperatureMode = false;
		uint32_t Temperature = 6570;
		glm::vec3 Color = glm::vec3(1.0f);
		float Intensity = 20.0f;

		float Range = 1.0f;
		float CutOffAngle = glm::radians(12.5f);
		float OuterCutOffAngle = glm::radians(17.5f);
		
		ShadowQualityType ShadowQuality = ShadowQualityType::UltraSoft;

		Ref<Framebuffer> ShadowMapFramebuffer;

		LightComponent()
		{
			const FramebufferSpecification spec{ 2048, 2048, { FramebufferTextureFormat::Depth } };
			ShadowMapFramebuffer =CreateRef<Framebuffer>(spec);
		}
	};


	///////////////////////////////////
	///////     UI Component    ///////
	///////////////////////////////////
	namespace UI
	{
		// RequireComponents [TransformComponent]
		// TODO: 여기서 spriteRenderer같은 거는 위와 중복이 되니, LayoutComponent를 이용해서 
		//       rendering할 때 projection만 바꾸는 방식으로 진행하자.
		struct LayoutComponent : public BaseComponent
		{
			friend class UISystem;

			glm::vec2 AnchorMin{0.0f, 0.0f};  // Anchor's minimum point normalized to parent size (0 to 1)
			glm::vec2 AnchorMax{1.0f, 1.0f};  // Anchor's maximum point normalized to parent size (0 to 1)
			glm::vec2 Pivot{0.5, 0.5};      // Pivot point normalized to the rectangle's size (0 to 1)

			// (x, y, w, h) or (l, t, r, b)
			// (x, t, w, b) or (l, y, r, h)
			// 1th and 3th element are related to x-axis
			// 2th and 4th element are related to y-axis
			glm::vec4 Box; 
			float Rotation = 0.0f;
			glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
			bool Interactable = false;

			// Controlled by UI System
			glm::vec2 _size;
		};

		struct SpriteRendererComponent : public BaseComponent
		{
			glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
			Ref<Texture2D> Texture = nullptr;
			int32_t SortingOrder = 0;
			float TilingFactor = 1.0f;
		};

		struct InputTextComponent : public BaseComponent
		{
			TextComponent text;
			SpriteRendererComponent caret;
		};

		struct ButtonComponent : public BaseComponent
		{
			friend class UISystem;
		

			std::function<void()> onClick;
			glm::vec4 overColor{1.0f};
			glm::vec4 pressColor{1.0f};
			glm::vec4 normalColor{1.0f};

		public:
			bool _over;
			bool _press;
			bool _release;
		};
	}
	
	
	// etc...
	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents = ComponentGroup<
		TransformComponent,
		RelationshipComponent,
		PrefabComponent,
		CameraComponent,

		// UI
		UI::LayoutComponent,
		UI::SpriteRendererComponent,
		UI::ButtonComponent,

		// 2D
		SpriteRendererComponent

		// 3D


	>;
} // namespace DawnStar
