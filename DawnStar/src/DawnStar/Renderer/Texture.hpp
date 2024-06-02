#pragma once

#include <memory>

namespace DawnStar
{

	enum class ImageFormat
	{
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void Invalidate(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		bool operator==(const Texture& other) const { return GetRendererID() == other.GetRendererID(); }
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D() = default;
		Texture2D(uint32_t width, uint32_t height);
		Texture2D(const std::string& path);
		~Texture2D() override;        

		uint32_t GetWidth() const override {return _width; }
		uint32_t GetHeight() const override {return _height; }
		uint32_t GetRendererID() const override {return _rendererID; }

		void SetData(void* data, uint32_t size) override;
		void Invalidate(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels) override;

		void Bind(uint32_t slot = 0) const override;      
	private:
		void InvalidateImpl(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels);  

	private:
		std::string _path;
		uint32_t _width, _height;
		uint32_t _rendererID;
		unsigned int _internalFormat, _dataFormat;
	};    

	
	class TextureCubemap : public Texture
	{
	public:
		TextureCubemap() = default;
		explicit TextureCubemap(const std::string& path);
		~TextureCubemap() override;

		TextureCubemap(const TextureCubemap& other) = default;
		TextureCubemap(TextureCubemap&& other) = default;
		
		virtual uint32_t GetWidth() const override {return _width; }
		virtual uint32_t GetHeight() const override {return _height; }
		virtual uint32_t GetRendererID() const override {return _rendererID; }
		uint64_t GetHRDRendererID() const { return _HRDRendererID; }
		const std::string& GetPath() const { return _path; }

		void SetData(void* data, uint32_t size) override;
		void Invalidate(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels) override;

		void Bind(uint32_t slot = 0) const override;
		void BindIrradianceMap(uint32_t slot) const;
		void BindRadianceMap(uint32_t slot) const;
		
	private:
		void InvalidateImpl(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels);

	private:
	
		std::string _path;
		uint32_t _width = 0, _height = 0;
		uint32_t _HRDRendererID = 0;
		uint32_t _rendererID = 0;
		uint32_t _irradianceRendererID = 0;
		uint32_t _radianceRendererID = 0;
		uint32_t _internalFormat = 0, _dataFormat = 0;
	};
} // namespace DawnStar
