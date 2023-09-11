#include <DawnStar/dspch.hpp>
#include <DawnStar/Renderer/VertexArray.hpp>

#include <glad/glad.h>

namespace DawnStar
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case DawnStar::ShaderDataType::Float:	return GL_FLOAT;
			case DawnStar::ShaderDataType::Float2:	return GL_FLOAT;
			case DawnStar::ShaderDataType::Float3:	return GL_FLOAT;
			case DawnStar::ShaderDataType::Float4:	return GL_FLOAT;
			case DawnStar::ShaderDataType::Mat3:	return GL_FLOAT;
			case DawnStar::ShaderDataType::Mat4:	return GL_FLOAT;
			case DawnStar::ShaderDataType::Int:	    return GL_INT;
			case DawnStar::ShaderDataType::Int2:	return GL_INT;
			case DawnStar::ShaderDataType::Int3:	return GL_INT;
			case DawnStar::ShaderDataType::Int4:	return GL_INT;
			case DawnStar::ShaderDataType::Bool:	return GL_BOOL;
		}

		DS_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	VertexArray::VertexArray()
	{
		//DS_PROFILE_FUNCTION();

		glCreateVertexArrays(1, &m_RendererID);
	}

	VertexArray::~VertexArray()
	{
		//DS_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VertexArray::Bind() const
	{
		//DS_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const
	{

		//DS_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		//DS_PROFILE_FUNCTION();

		DS_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset
			);
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);

	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		//DS_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}