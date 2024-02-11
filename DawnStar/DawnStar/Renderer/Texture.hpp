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

	struct TextureSpecification
	{
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		bool GenerateMips = true;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual bool IsLoaded() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		bool operator==(const Texture& other) const { return GetRendererID() == other.GetRendererID(); }
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(const TextureSpecification& specification);
		Texture2D(const std::string& path);
		virtual ~Texture2D();        
		
		const TextureSpecification& GetSpecification() const { return _specification; }

		virtual uint32_t GetWidth() const override {return _width; }
		virtual uint32_t GetHeight() const override {return _height; }
		virtual uint32_t GetRendererID() const override {return _rendererID; }

		virtual bool IsLoaded() const override { return _isLoaded; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;
        

	private:
		TextureSpecification _specification;
		std::string _path;

		uint32_t _width, _height;
		uint32_t _rendererID;
		unsigned int _internalFormat, _dataFormat;

		bool _isLoaded = false;
	};    
} // namespace DawnStar
