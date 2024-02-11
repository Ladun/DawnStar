#include <DawnStar/dspch.hpp>
#include <DawnStar/Renderer/Buffer.hpp>
#include <DawnStar/Renderer/Renderer.hpp>

#include <glad/glad.h>

namespace DawnStar
{
    
	/////////////////////////////////////////////////////////////////////////////////
	// ------------------- Vertex Buffer ----------------------------------------- //
	/////////////////////////////////////////////////////////////////////////////////

	VertexBuffer::VertexBuffer(uint32_t size)
	{
		DS_PROFILE_SCOPE()

		// glCreateBuffers(1, &_rendererID);
		glGenBuffers(1, &_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, _rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	VertexBuffer::VertexBuffer(float* vertices, uint32_t size)
	{
		// DS_PROFILE_SCOPE()

		// glCreateBuffers(1, &_rendererID);
		glGenBuffers(1, &_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, _rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		// DS_PROFILE_SCOPE()

		glDeleteBuffers(1, &_rendererID);
	}

	void VertexBuffer::Bind() const
	{
		// DS_PROFILE_SCOPE()

		glBindBuffer(GL_ARRAY_BUFFER, _rendererID);
	}

	void VertexBuffer::Unbind() const
	{
		// DS_PROFILE_SCOPE()

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, _rendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// ------------------- Index Buffer ------------------------------------------ //
	/////////////////////////////////////////////////////////////////////////////////

	IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count)
		: _count(count)
	{
		DS_PROFILE_SCOPE()

		// glCreateBuffers(1, &_rendererID);
		glGenBuffers(1, &_rendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _rendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}
	IndexBuffer::~IndexBuffer()
	{
		DS_PROFILE_SCOPE()

		glDeleteBuffers(1, &_rendererID);
	}
	void IndexBuffer::Bind() const
	{
		DS_PROFILE_SCOPE()

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _rendererID);
	}
	void IndexBuffer::Unbind() const
	{
		DS_PROFILE_SCOPE()

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
} // namespace DawnStar
