#include <DawnStar/dspch.hpp>
#include <DawnStar/Renderer/Texture.hpp>

#include <glad/glad.h>
#include <stb_image.h>

namespace DawnStar
{

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
		glTexImage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
		glTexImage2D(m_RendererID, 1, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
		glTexSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void Texture2D::Bind(uint32_t slot) const
	{
		DS_PROFILE_SCOPE()

		glBindTexture(slot, m_RendererID);
	}
}// namespace DawnStar