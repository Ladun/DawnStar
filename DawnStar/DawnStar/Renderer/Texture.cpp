#include <DawnStar/dspch.hpp>
#include <DawnStar/Renderer/Texture.hpp>

#include <glad/glad.h>
#include <stb_image.h>

namespace DawnStar
{
	// TODO: opengl3 버전으로 하느라 함수부분이 많이 다름( opengl4 버전과 )
	// 대표적은로 glTexture~라는 함수가 없어서 glTex~함수로 대신하는데,
	// 이 함수는 첫 매개변수로 GL_TEXTURE_2D를 받음
	// 하지만 glTexture~는 RendererID를 받는다~

    //////////////////////////////////////////////
    /////////         Texture2D         //////////
    //////////////////////////////////////////////
    Texture2D::Texture2D(uint32_t width, uint32_t height)
        :m_Width(width), m_Height(height)
    {
        
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		// glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		// glTexStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, NULL);
		
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    Texture2D::Texture2D(const std::string& path)
		: m_Path(path)
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

		m_Width = width;
		m_Height = height;

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

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		DS_CORE_ASSERT(internalFormat && dataFormat, "Format not supported!");

		glGenTextures(1, &m_RendererID);
		// glTexStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// glTexSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	Texture2D::~Texture2D()
	{
		DS_PROFILE_SCOPE()

		glDeleteTextures(1, &m_RendererID);
	}

	void Texture2D::SetData(void* data, uint32_t size)
	{
		DS_PROFILE_SCOPE()

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		DS_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}
		
	void Texture2D::Bind(uint32_t slot) const
	{
		DS_PROFILE_SCOPE()

		// TODO: this temporary code. need to change
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		// glBindTextureUnit(slot, m_RendererID);
	}
}// namespace DawnStar