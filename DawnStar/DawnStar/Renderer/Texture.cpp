#include <dspch.hpp>
#include <DawnStar/Renderer/Texture.hpp>

#include <glad/glad.h>
#include <stb_image.h>

#include <DawnStar/Renderer/Renderer3D.hpp>
#include <DawnStar/Renderer/Shader.hpp>

namespace DawnStar
{
	static Ref<Shader> _equirectangularToCubemapShader;
	static Ref<Shader> _irradianceShader;
	static Ref<Shader> _radianceShader;

	// TODO: opengl3 버전으로 하느라 함수부분이 많이 다름( opengl4 버전과 )
	// 대표적은로 glTexture~라는 함수가 없어서 glTex~함수로 대신하는데,
	// 이 함수는 첫 매개변수로 GL_TEXTURE_2D를 받음
	// 하지만 glTexture~는 RendererID를 받는다~

    //////////////////////////////////////////////
    /////////         Texture2D         //////////
    //////////////////////////////////////////////
	
	Texture2D::Texture2D(uint32_t width, uint32_t height)
		: _width(width), _height(height)
	{
		DS_PROFILE_SCOPE()
		
		_internalFormat = GL_RGBA8;
		_dataFormat = GL_RGBA;

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
			DS_PROFILE_SCOPE("stbi_load Texture");

			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		DS_CORE_ASSERT(data, "failed to load image!");

		InvalidateImpl(path, width, height, data, channels);

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
	void Texture2D::Invalidate(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels)
	{
		DS_PROFILE_SCOPE()

		InvalidateImpl(path, width, height, data, channels);
	}

	void Texture2D::InvalidateImpl(std::string_view path, uint32_t width, uint32_t height, const void * data, uint32_t channels)
	{
		DS_PROFILE_SCOPE()

		_path = path;

		if (_rendererID)
			glDeleteTextures(1, &_rendererID);

		_width = width;
		_height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		switch (channels)
		{
			case 1:
				internalFormat = GL_R8;
				dataFormat = GL_RED;
				break;
			case 2:
				internalFormat = GL_RG8;
				dataFormat = GL_RG;
				break;
			case 3:
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
				break;
			case 4:
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
				break;
			default:
				DS_CORE_ERROR("Texture channel count is not within (1-4) range. Channel count: {}", channels);
				break;
		}

		_internalFormat = internalFormat;
		_dataFormat = dataFormat;

		DS_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!")

		glGenTextures(1, &_rendererID);
		glBindTexture(GL_TEXTURE_2D, _rendererID);

#if DS_GL_HIGHER_VER
		glTextureParameteri(_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(_rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
#else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#endif

		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(internalFormat), static_cast<int>(_width), static_cast<int>(_height), 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void Texture2D::Bind(uint32_t slot) const
	{
		DS_PROFILE_SCOPE()

#if DS_GL_HIGHER_VER
		glBindTextureUnit(slot, _rendererID);
#else
		glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
		glBindTexture(GL_TEXTURE_2D, _rendererID);
#endif
	}
	
    //////////////////////////////////////////////
    /////////      TextureCubeMap       //////////
    //////////////////////////////////////////////

	TextureCubemap::TextureCubemap(const std::string& path)
	{
		DS_PROFILE_SCOPE()
		
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		float* data = nullptr;
		{
			DS_PROFILE_SCOPE("stbi_load Texture")
			
			data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
		}
		DS_CORE_ASSERT(data, "Failed to load image!")
		
		InvalidateImpl(path, width, height, data, channels);

		stbi_image_free(data);
	}

	TextureCubemap::~TextureCubemap()
	{
		DS_PROFILE_SCOPE()
		
		glDeleteTextures(1, &_HRDRendererID);
		glDeleteTextures(1, &_rendererID);
		glDeleteTextures(1, &_irradianceRendererID);
		glDeleteTextures(1, &_radianceRendererID);
	}

	void TextureCubemap::SetData( void* data,  uint32_t size)
	{
		DS_PROFILE_SCOPE()		
	}

	void TextureCubemap::Invalidate(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels)
	{
		DS_PROFILE_SCOPE()

		InvalidateImpl(path, width, height, data, channels);
	}

	void TextureCubemap::Bind(uint32_t slot) const
	{
		DS_PROFILE_SCOPE()
		
#if DS_GL_HIGHER_VER
		glBindTextureUnit(slot, _rendererID);
#else
		glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
		glBindTexture(GL_TEXTURE_2D, _rendererID);
#endif
	}

	void TextureCubemap::BindIrradianceMap(uint32_t slot) const
	{
		DS_PROFILE_SCOPE()

#if DS_GL_HIGHER_VER
		glBindTextureUnit(slot, _irradianceRendererID);
#else
		glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
		glBindTexture(GL_TEXTURE_2D, _irradianceRendererID);
#endif
	}

	void TextureCubemap::BindRadianceMap(uint32_t slot) const
	{
		DS_PROFILE_SCOPE()

#if DS_GL_HIGHER_VER
		glBindTextureUnit(slot, _radianceRendererID);
#else
		glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
		glBindTexture(GL_TEXTURE_2D, _radianceRendererID);
#endif
	}

	void TextureCubemap::InvalidateImpl(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels)
	{
		DS_PROFILE_SCOPE()

		if (channels < 3)
		{
			DS_CORE_ERROR("Couldn't load HDR cubemap with {} channels: {}", channels, path);
			return;
		}

		_path = path;

		constexpr uint32_t cubemapSize = 2048;
		constexpr uint32_t irradianceMapSize = 32;
		constexpr uint32_t radianceMapSize = cubemapSize / 4;

		_width = width;
		_height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		switch (channels)
		{
			case 1:
				internalFormat = GL_R16F;
				dataFormat = GL_RED;
				break;
			case 2:
				internalFormat = GL_RG16F;
				dataFormat = GL_RG;
				break;
			case 3:
				internalFormat = GL_RGB16F;
				dataFormat = GL_RGB;
				break;
			case 4:
				internalFormat = GL_RGBA16F;
				dataFormat = GL_RGBA;
				break;
			default:
				DS_CORE_ERROR("Texture channel count is not within (1-4) range. Channel count: {}", channels);
				break;
		}

		_internalFormat = internalFormat;
		_dataFormat = dataFormat;

		DS_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!")

		uint32_t captureFBO, captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubemapSize, cubemapSize);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		glGenTextures(1, &_HRDRendererID);
		glBindTexture(GL_TEXTURE_2D, _HRDRendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, static_cast<int>(_width), static_cast<int>(_height), 0, dataFormat, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenTextures(1, &_rendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _rendererID);
		for (int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, cubemapSize, cubemapSize, 0, dataFormat, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		const glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		const glm::mat4 captureViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		if (!_equirectangularToCubemapShader)
			_equirectangularToCubemapShader = CreateRef<Shader>("assets/shaders/EquirectangularToCubemap.glsl");

		_equirectangularToCubemapShader->Bind();
		_equirectangularToCubemapShader->SetInt("u_EquirectangularMap", 0);
		_equirectangularToCubemapShader->SetMat4("u_Projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _HRDRendererID);

		glViewport(0, 0, cubemapSize, cubemapSize); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		for (unsigned int i = 0; i < 6; ++i)
		{
			_equirectangularToCubemapShader->SetMat4("u_View", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _rendererID, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Renderer3D::DrawCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, _rendererID);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glGenTextures(1, &_irradianceRendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _irradianceRendererID);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, irradianceMapSize, irradianceMapSize, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradianceMapSize, irradianceMapSize);

		if (!_irradianceShader)
			_irradianceShader = CreateRef<Shader>("assets/shaders/Irradiance.glsl");

		_irradianceShader->Bind();
		_irradianceShader->SetInt("u_EnvironmentMap", 0);
		_irradianceShader->SetMat4("u_Projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _rendererID);

		glViewport(0, 0, irradianceMapSize, irradianceMapSize); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		for (unsigned int i = 0; i < 6; ++i)
		{
			_irradianceShader->SetMat4("u_View", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _irradianceRendererID, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Renderer3D::DrawCube();
		}

		// Radiance
		glGenTextures(1, &_radianceRendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _radianceRendererID);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, radianceMapSize, radianceMapSize, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		if (!_radianceShader)
			_radianceShader = CreateRef<Shader>("assets/shaders/PrefilterCubemap.glsl");
		_radianceShader->Bind();
		_radianceShader->SetInt("u_EnvironmentMap", 0);
		_radianceShader->SetMat4("u_Projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _rendererID);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		const int maxMipLevels = static_cast<int>(glm::log2(static_cast<float>(radianceMapSize))) + 1;
		constexpr auto radianceMapSizeFloat = static_cast<float>(radianceMapSize);
		for (int mip = 0; mip < maxMipLevels; ++mip)
		{
			const float m = radianceMapSizeFloat * static_cast<float>(glm::pow(0.5f, mip));
			const int sz = static_cast<int>(m);
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, sz, sz);
			glViewport(0, 0, sz, sz);

			const float roughness = static_cast<float>(mip) / static_cast<float>(maxMipLevels - 1);
			_radianceShader->SetFloat("u_Roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				_radianceShader->SetMat4("u_View", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _radianceRendererID, mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				Renderer3D::DrawCube();
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glDeleteFramebuffers(1, &captureFBO);
		glDeleteRenderbuffers(1, &captureRBO);
	}
}// namespace DawnStar