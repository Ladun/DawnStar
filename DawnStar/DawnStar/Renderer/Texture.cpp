#include <DawnStar/dspch.hpp>
#include <DawnStar/Renderer/Texture.hpp>

#include <glad/glad.h>
#include <stb_image.h>

namespace DawnStar
{

	namespace Utils {

		static GLenum DSImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB8:  return GL_RGB;
				case ImageFormat::RGBA8: return GL_RGBA;
			}

			DS_CORE_ASSERT(false);
			return 0;
		}
		
		static GLenum DSImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:  return GL_RGB8;
			case ImageFormat::RGBA8: return GL_RGBA8;
			}

			DS_CORE_ASSERT(false);
			return 0;
		}

	}

	// TODO: opengl3 버전으로 하느라 함수부분이 많이 다름( opengl4 버전과 )
	// 대표적은로 glTexture~라는 함수가 없어서 glTex~함수로 대신하는데,
	// 이 함수는 첫 매개변수로 GL_TEXTURE_2D를 받음
	// 하지만 glTexture~는 RendererID를 받는다~

    //////////////////////////////////////////////
    /////////         Texture2D         //////////
    //////////////////////////////////////////////
    Texture2D::Texture2D(const TextureSpecification& specification)
        :_specification(specification), _width(_specification.Width), _height(_specification.Height)
    {
		_internalFormat = Utils::DSImageFormatToGLInternalFormat(_specification.Format);
		_dataFormat = Utils::DSImageFormatToGLDataFormat(_specification.Format);

		// glCreateTextures(GL_TEXTURE_2D, 1, &_rendererID);
		// glTexStorage2D(_rendererID, 1, _internalFormat, _width, _height);
		glGenTextures(1, &_rendererID);
		glBindTexture(GL_TEXTURE_2D, _rendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _width, _height, 0, _dataFormat, GL_UNSIGNED_BYTE, NULL);
		
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

    Texture2D::Texture2D(const std::string& path)
		: _path(path)
	{
		DS_PROFILE_SCOPE()

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			// DS_PROFILE_FUNCTION("stbi_load - OpenGlTex2D::OpenGlTex2D(const std::string&)");

			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		DS_CORE_ASSERT(data, "failed to load image!");

		_isLoaded = true;

		_width = width;
		_height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		_internalFormat = internalFormat;
		_dataFormat = dataFormat;

		DS_CORE_ASSERT(internalFormat && dataFormat, "Format not supported!");

		glGenTextures(1, &_rendererID);
		// glTexStorage2D(_rendererID, 1, _internalFormat, _width, _height);
		glBindTexture(GL_TEXTURE_2D, _rendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// glTexSubImage2D(_rendererID, 0, 0, 0, _width, _height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);    
	}

	Texture2D::~Texture2D()
	{
		DS_PROFILE_SCOPE()

		glDeleteTextures(1, &_rendererID);
	}

	void Texture2D::SetData(void* data, uint32_t size)
	{
		DS_PROFILE_SCOPE()

		uint32_t bpp = _dataFormat == GL_RGBA ? 4 : 3;
		DS_CORE_ASSERT(size == _width * _height * bpp, "Data must be entire texture!");
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _rendererID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, _dataFormat, GL_UNSIGNED_BYTE, data);
	}

	void Texture2D::Bind(uint32_t slot) const
	{
		DS_PROFILE_SCOPE()

		// TODO: this temporary code. need to change
		glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
		glBindTexture(GL_TEXTURE_2D, _rendererID);
		// glBindTextureUnit(slot, _rendererID);
	}
}// namespace DawnStar