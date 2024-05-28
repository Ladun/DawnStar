#include <dspch.hpp>
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

	VertexBuffer::VertexBuffer(const float* vertices, uint32_t size)
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

	IndexBuffer::IndexBuffer(const uint32_t* indices, uint32_t count)
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
	/////////////////////////////////////////////////////////////////////////////////
	// ------------------- Uniform Buffer ------------------------------------------ //
	/////////////////////////////////////////////////////////////////////////////////

	UniformBuffer::UniformBuffer()
	{
		DS_PROFILE_SCOPE()

		// glCreateBuffers(1, &_rendererID);
		glGenBuffers(1, &_rendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, _rendererID);
	}

	UniformBuffer::~UniformBuffer()
	{
		DS_PROFILE_SCOPE()

		glDeleteBuffers(1, &_rendererID);

	}

    void UniformBuffer::Bind() const
    {
		DS_PROFILE_SCOPE()

		glBindBuffer(GL_UNIFORM_BUFFER, _rendererID);
    }

    void UniformBuffer::Unbind() const
    {
		DS_PROFILE_SCOPE()

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UniformBuffer::SetData(const void *data, uint32_t offset, uint32_t size)
    {
		DS_PROFILE_SCOPE()

		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    }

    void UniformBuffer::SetLayout(const BufferLayout &layout, uint32_t blockIndex, uint32_t count)
    {
		DS_PROFILE_SCOPE()

		_layout = layout;

		uint32_t size = 0;
		for(const auto& element : _layout)
			size += ShaderDataTypeSize(element.Type);

		size *= count;

		glBindBuffer(GL_UNIFORM_BUFFER, _rendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, blockIndex, _rendererID, 0, size);

    }

} // namespace DawnStar
