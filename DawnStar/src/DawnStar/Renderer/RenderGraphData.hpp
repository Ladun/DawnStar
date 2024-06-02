#pragma once

#include <DawnStar/Renderer/FrameBuffer.hpp>

namespace DawnStar
{
	struct CameraData
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::mat4 ViewProjection;
		glm::vec3 Position;
	};

    struct RenderGraphData
	{
		Ref<Framebuffer> CompositePassTarget;
		Ref<Framebuffer> RenderPassTarget;
		Ref<Framebuffer> LightingPassTarget;
		Ref<Framebuffer> FXAAPassTarget;

		static constexpr size_t MaxBlurSamples = 6;
		size_t BlurSamples = MaxBlurSamples;
		Ref<Framebuffer> PrefilteredFramebuffer;
		Ref<Framebuffer> TempBlurFramebuffers[MaxBlurSamples];
		Ref<Framebuffer> DownsampledFramebuffers[MaxBlurSamples];
		Ref<Framebuffer> UpsampledFramebuffers[MaxBlurSamples];

		RenderGraphData(uint32_t width, uint32_t height)
		{
			DS_PROFILE_SCOPE()

			if (width <= 0 || height <= 0)
			{
				DS_CORE_ERROR("Cannot create a RenderGraph with {}x{} dimensions, making it 1x1", width, height);
				width = 1;
				height = 1;
			}
			
			{
				FramebufferSpecification spec;
				spec.Attachments = { FramebufferTextureFormat::R11G11B10 };
				spec.Width = width;
				spec.Height = height;
				CompositePassTarget = CreateRef<Framebuffer>(spec);
			}

			{
				FramebufferSpecification spec;
				spec.Attachments = {
					FramebufferTextureFormat::RGBA8,				// Albedo
					FramebufferTextureFormat::RG16F,				// Normal
					FramebufferTextureFormat::RGBA8,				// Metallic, Roughness, AO
					FramebufferTextureFormat::RGBA8,				// rgb: EmissionColor, a: intensity
					FramebufferTextureFormat::Depth
				};
				spec.Width = width;
				spec.Height = height;
				RenderPassTarget = CreateRef<Framebuffer>(spec);
			}

			{
				FramebufferSpecification spec;
				spec.Attachments = { FramebufferTextureFormat::R11G11B10 };
				spec.Width = width;
				spec.Height = height;
				LightingPassTarget = CreateRef<Framebuffer>(spec);
				FXAAPassTarget = CreateRef<Framebuffer>(spec);
			}

			width /= 2;
			height /= 2;
			FramebufferSpecification bloomSpec;
			bloomSpec.Attachments = { FramebufferTextureFormat::R11G11B10 };
			bloomSpec.Width = width;
			bloomSpec.Height = height;
			PrefilteredFramebuffer = CreateRef<Framebuffer>(bloomSpec);

			BlurSamples = 0;
			for (size_t i = 0; i < MaxBlurSamples; i++)
			{
				++BlurSamples;

				width /= 2;
				height /= 2;

				if (width <= 0 || height <= 0)
					break;

				FramebufferSpecification blurSpec;
				blurSpec.Attachments = { FramebufferTextureFormat::R11G11B10 };
				blurSpec.Width = width;
				blurSpec.Height = height;
				TempBlurFramebuffers[i] = CreateRef<Framebuffer>(bloomSpec);
				DownsampledFramebuffers[i] = CreateRef<Framebuffer>(blurSpec);
				UpsampledFramebuffers[i] = CreateRef<Framebuffer>(blurSpec);
			}
		}

		void Resize(uint32_t width, uint32_t height)
		{
			DS_PROFILE_SCOPE()

			CompositePassTarget->Resize(width, height);
			RenderPassTarget->Resize(width, height);
			LightingPassTarget->Resize(width, height);
			FXAAPassTarget->Resize(width, height);

			width /= 2;
			height /= 2;
			PrefilteredFramebuffer->Resize(width, height);

			BlurSamples = 0;
			for (size_t i = 0; i < MaxBlurSamples; i++)
			{
				++BlurSamples;

				width /= 2;
				height /= 2;

				if (width <= 0 || height <= 0)
					break;

				TempBlurFramebuffers[i]->Resize(width, height);
				DownsampledFramebuffers[i]->Resize(width, height);
				UpsampledFramebuffers[i]->Resize(width, height);
			}
		}
	};
} // namespace DawnStar
