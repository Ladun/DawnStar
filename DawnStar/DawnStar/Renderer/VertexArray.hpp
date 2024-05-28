#pragma once

#include <memory>
#include <DawnStar/Renderer/Buffer.hpp>

namespace DawnStar
{
	class VertexArray
	{

	public:
		VertexArray();
		~VertexArray();

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

		const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return _vertexBuffers; }
		const Ref<IndexBuffer>& GetIndexBuffer() const { return _indexBuffer; }

	private:
		unsigned int _rendererID;
		std::vector<Ref<VertexBuffer>> _vertexBuffers;
		Ref<IndexBuffer> _indexBuffer;

	};
}