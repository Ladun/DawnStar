#include <DawnStar/dspch.hpp>
#include <DawnStar/Renderer/RendererAPI.hpp>

#include <glad/glad.h>


namespace DawnStar
{
	void OpenGLMessageCallback(
		[[maybe_unused]] unsigned source,
		[[maybe_unused]] unsigned type,
		[[maybe_unused]] unsigned id,
		[[maybe_unused]] unsigned severity,
		[[maybe_unused]] int length,
		[[maybe_unused]] const char* message,
		[[maybe_unused]] const void* userParam)
	{
		// For OpenGL 4.5
		// switch (severity)
		// {
			
		// 	case GL_DEBUG_SEVERITY_HIGH:			DS_CORE_CRITICAL(message); return;
		// 	case GL_DEBUG_SEVERITY_MEDIUM:			DS_CORE_ERROR(message); return;
		// 	case GL_DEBUG_SEVERITY_LOW:				DS_CORE_WARN(message); return;
		// 	case GL_DEBUG_SEVERITY_NOTIFICATION:	DS_CORE_TRACE(message); return;
		// 	default:								DS_CORE_DEBUG("Unknown severity level: {}", message);
		// }
	}

	void RendererAPI::Init()
	{
		DS_PROFILE_SCOPE()

		// For OpenGL 4.5
		// #ifdef DS_DEBUG
		// 	glEnable(GL_DEBUG_OUTPUT);
		// 	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		// 	glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		// 	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		// #endif


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		// glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void RendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void RendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		uint32_t count = indexCount ? vertexArray->GetIndexBuffer()->GetCount(): indexCount;
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
void RendererAPI::Draw(const Ref<VertexArray>& vertexArray, uint32_t count)
	{
		// DS_PROFILE_SCOPE()

		vertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(count));
	}

	void RendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		// DS_PROFILE_SCOPE()

		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, static_cast<int>(vertexCount));
	}

	void RendererAPI::EnableCulling()
	{
		// DS_PROFILE_SCOPE()

		glEnable(GL_CULL_FACE);
	}

	void RendererAPI::DisableCulling()
	{
		// DS_PROFILE_SCOPE()

		glDisable(GL_CULL_FACE);
	}

	void RendererAPI::FrontCull()
	{
		// DS_PROFILE_SCOPE()

		glCullFace(GL_FRONT);
	}

	void RendererAPI::BackCull()
	{
		// DS_PROFILE_SCOPE()

		glCullFace(GL_BACK);
	}

	void RendererAPI::SetDepthMask(bool value)
	{
		// DS_PROFILE_SCOPE()

		glDepthMask(value);
	}

	void RendererAPI::SetDepthTest(bool value)
	{
		// DS_PROFILE_SCOPE()

		if (value)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void RendererAPI::SetBlendState(bool value)
	{
		if (value)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}
}