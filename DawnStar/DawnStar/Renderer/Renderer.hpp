#pragma once

#include <DawnStar/Renderer/RenderCommand.hpp>

#include <DawnStar/Renderer/Camera.hpp>
#include <DawnStar/Renderer/Shader.hpp>

namespace DawnStar
{
    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void OnWindowResize(uint32_t width, uint32_t height);

        static void BeginScene(Camera& camera);
        static void EndScene();

        static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
    };
} // namespace DawnStar
