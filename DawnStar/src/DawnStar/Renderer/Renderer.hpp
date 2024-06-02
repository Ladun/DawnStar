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
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* _sceneData;
    };
} // namespace DawnStar
