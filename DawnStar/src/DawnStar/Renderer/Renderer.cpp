#include <dspch.hpp>
#include <DawnStar/Renderer/Renderer.hpp>

#include <DawnStar/Renderer/Renderer2D.hpp>
#include <DawnStar/Renderer/Renderer3D.hpp>

namespace DawnStar
{
    Renderer::SceneData* Renderer::_sceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		DS_PROFILE_SCOPE()

		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();
	}

	void Renderer::Shutdown()
	{
		DS_PROFILE_SCOPE()

		Renderer2D::Shutdown();
		Renderer3D::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		DS_PROFILE_SCOPE()

		RenderCommand::SetViewport(0, 0, width, height);
	}
} // namespace DawnStar
