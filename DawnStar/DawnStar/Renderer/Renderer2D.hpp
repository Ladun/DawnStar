#pragma once

#include <DawnStar/Core/Core.hpp>
#include <DawnStar/Renderer/Font.hpp>
#include <DawnStar/Scene/Components.hpp>

#include <glm/glm.hpp>

namespace DawnStar
{
    
	class Texture2D;
	struct RenderGraphData;

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		
		static void BeginScene(const glm::mat4& viewProjection);
		static void EndScene();//const Ref<RenderGraphData>& renderGraphData);
		static void Flush();

		//Primitives
		static void DrawQuad(const glm::vec2& position, float rotation, const glm::vec2& size, const Ref<Texture2D>& texture = nullptr, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);
		static void DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const Ref<Texture2D>& texture = nullptr, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture = nullptr, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);

		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color);

		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color);

		struct TextParams
		{
			glm::vec4 Color{ 1.0f };
			float Kerning = 0.0f;
			float LineSpacing = 0.0f;
		};
		static void DrawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams);
		static void DrawString(const glm::mat4& transform, const TextComponent& component);

		// Stats
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			[[nodiscard]] uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
			[[nodiscard]] uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
			[[nodiscard]] uint32_t GetTotalTriangleCount() const { return QuadCount * 2; }
		};

		static void ResetStats();
		[[nodiscard]] static Statistics GetStats();
	private:
		static void StartBatch();
		static void NextBatch();
	};
} // namespace DawnStar
