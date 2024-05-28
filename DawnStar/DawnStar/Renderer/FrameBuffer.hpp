#pragma once

namespace DawnStar
{
    enum class FramebufferTextureFormat
    {        
		None = 0,

		// Color
		RGBA8,
		RGBA16F,
		RGBA32F,
		R11G11B10,
		RG16F,
		R32I,

		// Depth/Stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
    };

    struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO: filtering/wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

    struct Framebuffer
    {
    public:
        Framebuffer(const FramebufferSpecification& spec);
        ~Framebuffer();

		void Invalidate();

        void Bind();
        void Unbind();

        void BindColorAttachment(uint32_t index, uint32_t slot);
        void BindDepthAttachment(uint32_t slot);

        void Resize(uint32_t witdh, uint32_t height);

        uint64_t GetColorAttachmentRendererID(uint32_t index=0) const { DS_CORE_ASSERT(index < _colorAttachments.size()) return _colorAttachments[index]; }
        uint64_t GetDepthAttachmentRendererID() const { return _depthAttachment; }

        const FramebufferSpecification& GetSpecification() const { return _specification; }
        
	private:
		uint32_t _rendererID = 0;
		FramebufferSpecification _specification;
		std::vector<FramebufferTextureSpecification> _colorAttachmentSpecifications;
		FramebufferTextureSpecification _depthAttachmentSpecification;

		std::vector<uint32_t> _colorAttachments;
		uint32_t _depthAttachment = 0;
    };

} // namespace DawnStar
