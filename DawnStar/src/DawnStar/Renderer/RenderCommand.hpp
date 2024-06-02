#pragma once

#include <DawnStar/Renderer/RendererAPI.hpp>

namespace DawnStar
{
	class RenderCommand
	{
	public:

		inline static void Init()
		{
			_rendererAPI->Init();
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			_rendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			_rendererAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			_rendererAPI->Clear();
		}
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			_rendererAPI->DrawIndexed(vertexArray, indexCount);
		}

		inline static void Draw(const Ref<VertexArray>& vertexArray, uint32_t count)
		{
			_rendererAPI->Draw(vertexArray, count);
		}
		
		inline static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			_rendererAPI->DrawLines(vertexArray, vertexCount);
		}

		inline static void EnableCulling()
		{
			_rendererAPI->EnableCulling();
		}

		inline static void DisableCulling()
		{
			_rendererAPI->DisableCulling();
		}

		inline static void FrontCull()
		{
			_rendererAPI->FrontCull();
		}

		inline static void BackCull()
		{
			_rendererAPI->BackCull();
		}

		inline static void SetDepthMask(bool value)
		{
			_rendererAPI->SetDepthMask(value);
		}

		inline static void SetDepthTest(bool value)
		{
			_rendererAPI->SetDepthTest(value);
		}

		inline static void SetBlendState(bool value)
		{
			_rendererAPI->SetBlendState(value);
		}
	private:
		static Scope<RendererAPI> _rendererAPI;

	};
}