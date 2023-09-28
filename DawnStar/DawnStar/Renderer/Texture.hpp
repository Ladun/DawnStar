#pragma once

#include <memory>

namespace DawnStar
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		bool operator==(const Texture& other) const { return GetRendererID() == other.GetRendererID(); }
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(uint32_t width, uint32_t height);
		Texture2D(const std::string& path);
		virtual ~Texture2D();
        

		virtual uint32_t GetWidth() const override {return m_Width; }
		virtual uint32_t GetHeight() const override {return m_Height; }
		virtual uint32_t GetRendererID() const override {return m_RendererID; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;
        

	private:
		std::string m_Path;

		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		unsigned int m_InternalFormat, m_DataFormat;
	};    
} // namespace DawnStar
