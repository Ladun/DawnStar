#include <dspch.hpp>
#include <DawnStar/Renderer/FrameBuffer.hpp>

#include <glad/glad.h>

namespace DawnStar
{
	static constexpr uint32_t s_MaxFramebufferSize = 8192;


	namespace Utils
	{
		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			return GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			DS_PROFILE_SCOPE()

			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t id)
		{
			DS_PROFILE_SCOPE()

			glBindTexture(TextureTarget(multisampled), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			DS_PROFILE_SCOPE()

			const bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			DS_PROFILE_SCOPE()

			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			return format == FramebufferTextureFormat::DEPTH24STENCIL8;
		}
	}

    Framebuffer::Framebuffer(const FramebufferSpecification &spec)
        : _specification(spec)
    {
        DS_PROFILE_SCOPE()

        for(const auto& s : _specification.Attachments.Attachments)
        {
            if(!Utils::IsDepthFormat(s.TextureFormat))
                _colorAttachmentSpecifications.emplace_back(s);
            else
                _depthAttachmentSpecification = s;
        }

        Invalidate();
        
    }
    Framebuffer::~Framebuffer()
    {
        DS_PROFILE_SCOPE()

        glDeleteFramebuffers(1, &_rendererID);
		glDeleteTextures(static_cast<int>(_colorAttachments.size()), _colorAttachments.data());
		glDeleteTextures(1, &_depthAttachment);
    }


    void Framebuffer::Invalidate()
    {
        DS_PROFILE_SCOPE()

        if(_rendererID)
        {
            
			DS_PROFILE_SCOPE("FramebufferDelete")

            glDeleteFramebuffers(1, &_rendererID);
            glDeleteTextures(static_cast<int>(_colorAttachments.size()), _colorAttachments.data());
            glDeleteTextures(1, &_depthAttachment);

            _colorAttachments.clear();
            _depthAttachment = 0;
        }

        {
            DS_PROFILE_SCOPE("FramebufferCreate")
            
			glCreateFramebuffers(1, &_rendererID);
            glBindFramebuffer(GL_FRAMEBUFFER, _rendererID);
        }

        {
            DS_PROFILE_SCOPE("FramebufferConfigureAttachments")

            const bool multisample = _specification.Samples > 1;

            if(!_colorAttachmentSpecifications.empty())
            {
                _colorAttachments.resize(_colorAttachmentSpecifications.size());
                const auto colorAttachmentSize = static_cast<int32_t>(_colorAttachments.size());
                Utils::CreateTextures(multisample, _colorAttachments.data(), colorAttachmentSize);

                // Attach color texture to current framebuffer
                for(int i = 0; i < colorAttachmentSize; i++)
                {
                    Utils::BindTexture(multisample, _colorAttachments[i]);
                    switch(_colorAttachmentSpecifications[i].TextureFormat)
                    {
						case FramebufferTextureFormat::RGBA8:
							Utils::AttachColorTexture(_colorAttachments[i], static_cast<int>(_specification.Samples), GL_RGBA8, GL_RGBA, _specification.Width, _specification.Height, i);
							break;
						case FramebufferTextureFormat::RGBA16F:
							Utils::AttachColorTexture(_colorAttachments[i], static_cast<int>(_specification.Samples), GL_RGBA16F, GL_RGBA, _specification.Width, _specification.Height, i);
							break;
						case FramebufferTextureFormat::RGBA32F:
							Utils::AttachColorTexture(_colorAttachments[i], static_cast<int>(_specification.Samples), GL_RGBA32F, GL_RGBA, _specification.Width, _specification.Height, i);
							break;
						case FramebufferTextureFormat::R11G11B10:
							Utils::AttachColorTexture(_colorAttachments[i], static_cast<int>(_specification.Samples), GL_R11F_G11F_B10F, GL_RGB, _specification.Width, _specification.Height, i);
							break;
						case FramebufferTextureFormat::RG16F:
							Utils::AttachColorTexture(_colorAttachments[i], static_cast<int>(_specification.Samples), GL_RG16F, GL_RG, _specification.Width, _specification.Height, i);
							break;
						case FramebufferTextureFormat::R32I:
							Utils::AttachColorTexture(_colorAttachments[i], static_cast<int>(_specification.Samples), GL_R32I, GL_RED_INTEGER, _specification.Width, _specification.Height, i);
							break;
						default:
							DS_CORE_ASSERT(false)
							break;
                    }
                }

            }

			if (_depthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
			{
				Utils::CreateTextures(multisample, &_depthAttachment, 1);
				Utils::BindTexture(multisample, _depthAttachment);
				switch (_depthAttachmentSpecification.TextureFormat)
				{
					case FramebufferTextureFormat::DEPTH24STENCIL8:
						Utils::AttachDepthTexture(_depthAttachment, static_cast<int>(_specification.Samples), GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, _specification.Width, _specification.Height);
						break;
					default:
						DS_CORE_ASSERT(false)
						break;
				}
			}

            if(_colorAttachmentSpecifications.size() > 1)
            {
                DS_CORE_ASSERT(_colorAttachments.size() <= 5)
                constexpr GLenum buffers[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
                glDrawBuffers(static_cast<int>(_colorAttachments.size()), buffers);
            }
            else if(_colorAttachments.empty())
            {
                glDrawBuffer(GL_NONE);
            }
            
			DS_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!")
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Bind()
    {
        DS_PROFILE_SCOPE()
        
		glBindFramebuffer(GL_FRAMEBUFFER, _rendererID);
		glViewport(0, 0, static_cast<int>(_specification.Width), static_cast<int>(_specification.Height));
    }

    void Framebuffer::Unbind()
    {
        DS_PROFILE_SCOPE()

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::BindColorAttachment(uint32_t index, uint32_t slot)
    {
        DS_PROFILE_SCOPE()

        DS_CORE_ASSERT(index < _colorAttachments.size())
		glBindTextureUnit(slot, _colorAttachments[index]);
    }

    void Framebuffer::BindDepthAttachment(uint32_t slot)
    {
        DS_PROFILE_SCOPE()

		glBindTextureUnit(slot, _depthAttachment);
    }

    void Framebuffer::Resize(uint32_t width, uint32_t height)
    {
        DS_PROFILE_SCOPE()

		if(width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			DS_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
			return;
		}
		
		_specification.Width = width;
		_specification.Height = height;
		
		Invalidate();
    }
} // namespace DawnStar
