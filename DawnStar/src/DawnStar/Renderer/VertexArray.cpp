#include <dspch.hpp>
#include <DawnStar/Renderer/VertexArray.hpp>

#include <glad/glad.h>

namespace DawnStar
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case DawnStar::ShaderDataType::Float:	
			case DawnStar::ShaderDataType::Float2:	
			case DawnStar::ShaderDataType::Float3:	
			case DawnStar::ShaderDataType::Float4:	
			case DawnStar::ShaderDataType::Mat3:	
			case DawnStar::ShaderDataType::Mat4:	return GL_FLOAT;
			case DawnStar::ShaderDataType::Int:	    
			case DawnStar::ShaderDataType::Int2:	
			case DawnStar::ShaderDataType::Int3:	
			case DawnStar::ShaderDataType::Int4:	return GL_INT;
			case DawnStar::ShaderDataType::Bool:	return GL_BOOL;
		}

		DS_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	VertexArray::VertexArray()
	{
		DS_PROFILE_SCOPE();

		glGenVertexArrays(1, &_rendererID);
	}

	VertexArray::~VertexArray()
	{
		DS_PROFILE_SCOPE();

		glDeleteVertexArrays(1, &_rendererID);
	}

	void VertexArray::Bind() const
	{
		DS_PROFILE_SCOPE();

		glBindVertexArray(_rendererID);
	}

	void VertexArray::Unbind() const
	{

		DS_PROFILE_SCOPE();

		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		DS_PROFILE_SCOPE();

		DS_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(_rendererID);
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

		_vertexBuffers.push_back(vertexBuffer);

	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		DS_PROFILE_SCOPE();

		glBindVertexArray(_rendererID);
		indexBuffer->Bind();

		_indexBuffer = indexBuffer;
	}
}