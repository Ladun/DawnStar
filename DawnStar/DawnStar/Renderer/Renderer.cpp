#include <DawnStar/dspch.hpp>
#include <DawnStar/Renderer/Renderer.hpp>

#include <DawnStar/Renderer/Renderer2D.hpp>

namespace DawnStar
{
    Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		//DS_PROFILE_FUNCTION();
		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}
} // namespace DawnStar
