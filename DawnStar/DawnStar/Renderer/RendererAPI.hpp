#pragma once

#include <glm/glm.hpp>
#include <DawnStar/Renderer/VertexArray.hpp>

namespace DawnStar
{
	class RendererAPI
	{
	public:
		void Init();
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		void SetClearColor(const glm::vec4& color);
		void Clear();

		void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount);
		void Draw(const Ref<VertexArray>& vertexArray, uint32_t count);
		void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount);
		
		void EnableCulling();
		void DisableCulling();
		void FrontCull();
		void BackCull();
		void SetDepthMask(bool value);
		void SetDepthTest(bool value);
		void SetBlendState(bool value);
	};
}