#include <DawnStar/dspch.hpp>
#include <DawnStar/Renderer/RenderCommand.hpp>

namespace DawnStar
{
    Scope<RendererAPI> RenderCommand::s_RendererAPI = CreateScope<RendererAPI>();
} // namespace DawnStar
