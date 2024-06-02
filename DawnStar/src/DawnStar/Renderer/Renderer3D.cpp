#include <dspch.hpp>
#include <DawnStar/Renderer/Renderer3D.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <DawnStar/Renderer/Renderer.hpp>
#include <DawnStar/Renderer/RenderGraphData.hpp>
#include <DawnStar/Renderer/FrameBuffer.hpp>
#include <DawnStar/Renderer/VertexArray.hpp>
#include <DawnStar/Renderer/Material.hpp>
#include <DawnStar/Renderer/Shader.hpp>

#include <DawnStar/Scene/Entity.hpp>


namespace DawnStar
{
    Renderer3D::Statistics Renderer3D::_stats;
	std::vector<Renderer3D::MeshData> Renderer3D::_meshes;
	Ref<Texture2D>  Renderer3D::_BRDFLutTexture;
	Ref<Shader>     Renderer3D::_shader;
	Ref<Shader>     Renderer3D::_lightingShader;
	Ref<Shader>     Renderer3D::_shadowMapShader;
	Ref<Shader>     Renderer3D::_cubemapShader;
	Ref<Shader>     Renderer3D::_gaussianBlurShader;
	Ref<Shader>     Renderer3D::_fxaaShader;
	Ref<Shader>     Renderer3D::_hdrShader;
	Ref<Shader>     Renderer3D::_bloomShader;
	Ref<VertexArray> Renderer3D::_quadVertexArray;
	Ref<VertexArray> Renderer3D::_cubeVertexArray;
	Ref<UniformBuffer> Renderer3D::_ubCamera;
	Ref<UniformBuffer> Renderer3D::_ubPointLights;
	Ref<UniformBuffer> Renderer3D::_ubDirectionalLights;

    Entity Renderer3D::_skylight;
    std::vector<Entity> Renderer3D::_sceneLights;
    
	ShaderLibrary Renderer3D::_shaderLibrary;
	Renderer3D::TonemappingType Renderer3D::Tonemapping = Renderer3D::TonemappingType::ACES;
	float Renderer3D::Exposure = 1.0f;
	bool  Renderer3D::UseBloom = true;
	float Renderer3D::BloomStrength = 0.1f;
	float Renderer3D::BloomThreshold = 1.0f;
	float Renderer3D::BloomKnee = 0.1f;
	float Renderer3D::BloomClamp = 100.0f;
	bool  Renderer3D::UseFXAA = true;
	glm::vec2 Renderer3D::FXAAThreshold = glm::vec2(0.0078125f, 0.125f);		// x: current threshold, y: relative threshold
	glm::vec4 Renderer3D::VignetteColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.25f);	// rgb: color, a: intensity
	glm::vec4 Renderer3D::VignetteOffset = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);	// xy: offset, z: useMask, w: enable/disable effect
	Ref<Texture2D> Renderer3D::VignetteMask = nullptr;

	void Renderer3D::Init()
    {
        DS_PROFILE_SCOPE()

        _ubCamera = CreateRef<UniformBuffer>();
		// _ubCamera->SetLayout({
		// 	{ ShaderDataType::Mat4, "u_View" },
		// 	{ ShaderDataType::Mat4, "u_Projection" },
		// 	{ ShaderDataType::Mat4, "u_ViewProjection" },
		// 	{ ShaderDataType::Float4, "u_CameraPosition" }
		// }, 0);

		_ubPointLights = CreateRef<UniformBuffer>();
		_ubPointLights->SetLayout({
			{ ShaderDataType::Float4, "u_Position" },
			{ ShaderDataType::Float4, "u_Color" },
			{ ShaderDataType::Float4, "u_AttenFactors" },
			{ ShaderDataType::Float4, "u_LightDir" },
		}, 1, MAX_NUM_LIGHTS + 1);

		_ubDirectionalLights = CreateRef<UniformBuffer>();
		_ubDirectionalLights->SetLayout({
			{ ShaderDataType::Float4, "u_Position" },
			{ ShaderDataType::Float4, "u_Color" },
			{ ShaderDataType::Float4, "u_LightDir" },
			{ ShaderDataType::Mat4, "u_DirLightViewProj" },
		}, 2, MAX_NUM_DIR_LIGHTS + 1);

		_BRDFLutTexture = CreateRef<Texture2D>("assets/resources/BRDF_LUT.jpg");

		_shaderLibrary      = ShaderLibrary();
		_shadowMapShader    = _shaderLibrary.Load("assets/shaders/DepthShader.glsl");
		_cubemapShader      = _shaderLibrary.Load("assets/shaders/Cubemap.glsl");
		_gaussianBlurShader = _shaderLibrary.Load("assets/shaders/GaussianBlur.glsl");
		_fxaaShader         = _shaderLibrary.Load("assets/shaders/FXAA.glsl");
		_hdrShader          = _shaderLibrary.Load("assets/shaders/HDR.glsl");
		_bloomShader        = _shaderLibrary.Load("assets/shaders/Bloom.glsl");
		_shader             = _shaderLibrary.Load("assets/shaders/PBR.glsl");
		_lightingShader     = _shaderLibrary.Load("assets/shaders/LightingPass.glsl");

        _shader->Bind();
        _shader->SetUniformBlock("Camera", 0);

        _cubemapShader->Bind();
        _cubemapShader->SetUniformBlock("Camera", 0);

        _lightingShader->Bind();
        _lightingShader->SetUniformBlock("Camera", 0);
        _lightingShader->SetUniformBlock("PointLightBuffer", 1);
        _lightingShader->SetUniformBlock("DirectionalLightBuffer", 2);

        // Cube-map
		{
			constexpr float vertices[108] = {
				// back face
				 0.5f, -0.5f, -0.5f,
				 0.5f,  0.5f, -0.5f,
				-0.5f, -0.5f, -0.5f,

				-0.5f,  0.5f, -0.5f,
				-0.5f, -0.5f, -0.5f,
				 0.5f,  0.5f, -0.5f,
			
				 // front face
				 0.5f,  0.5f,  0.5f,
				 0.5f, -0.5f,  0.5f,
				-0.5f, -0.5f,  0.5f,

				-0.5f, -0.5f,  0.5f,
				-0.5f,  0.5f,  0.5f,
				 0.5f,  0.5f,  0.5f,
			
				 // left face
				-0.5f, -0.5f, -0.5f,
				-0.5f,  0.5f, -0.5f,
				-0.5f,  0.5f,  0.5f,
			
				-0.5f,  0.5f,  0.5f,
				-0.5f, -0.5f,  0.5f,
				-0.5f, -0.5f, -0.5f,
				// right face
				 0.5f,  0.5f, -0.5f,
				 0.5f, -0.5f, -0.5f,
				 0.5f,  0.5f,  0.5f,

				 0.5f, -0.5f,  0.5f,
				 0.5f,  0.5f,  0.5f,
				 0.5f, -0.5f, -0.5f,
				// bottom face
				 0.5f, -0.5f,  0.5f,
				 0.5f, -0.5f, -0.5f,
				-0.5f, -0.5f, -0.5f,

				-0.5f, -0.5f, -0.5f,
				-0.5f, -0.5f,  0.5f,
				 0.5f, -0.5f,  0.5f,
				// top face
				 0.5f,  0.5f, -0.5f,
				 0.5f,  0.5f , 0.5f,
				-0.5f,  0.5f, -0.5f,

				-0.5f,  0.5f,  0.5f,
				-0.5f,  0.5f, -0.5f,
				 0.5f,  0.5f,  0.5f,
				};

			Ref<VertexBuffer> cubeVertexBuffer = CreateRef<VertexBuffer>(vertices, 108 * sizeof(float));
			cubeVertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" }
			});

			_cubeVertexArray = CreateRef<VertexArray>();
			_cubeVertexArray->AddVertexBuffer(cubeVertexBuffer);
		}

        // Quad
		{
			constexpr float vertices[20] = {
				 -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				  1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				  1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
			};

			constexpr uint32_t indices[6] = {
				0, 1, 2,
				0, 2, 3
			};

			_quadVertexArray = CreateRef<VertexArray>();

			Ref<VertexBuffer> quadVertexBuffer = CreateRef<VertexBuffer>(vertices, 20 * sizeof(float));
			quadVertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" }
			});
			_quadVertexArray->AddVertexBuffer(quadVertexBuffer);

			Ref<IndexBuffer> quadIndexBuffer = CreateRef<IndexBuffer>(indices, 6);
			_quadVertexArray->SetIndexBuffer(quadIndexBuffer);
		}
    }

    void Renderer3D::Shutdown() 
	{
		DS_PROFILE_SCOPE()

	}

	void Renderer3D::BeginScene(const CameraData& cameraData, Entity cubeMap, std::vector<Entity>&& lights)
	{
		DS_PROFILE_SCOPE()

		_skylight = cubeMap;
		_sceneLights = std::move(lights);

		SetupCameraData(cameraData);
		SetupLightsData();
	}

	void Renderer3D::EndScene(const Ref<RenderGraphData>& renderTarget)
	{
		DS_PROFILE_SCOPE()

		Flush(renderTarget);

		_shader->Unbind();
	}
	
	void Renderer3D::DrawCube()
	{
		DS_PROFILE_SCOPE()

		RenderCommand::Draw(_cubeVertexArray, 36);
	}

	void Renderer3D::DrawQuad()
	{
		DS_PROFILE_SCOPE()

		RenderCommand::DrawIndexed(_quadVertexArray);
	}

	void Renderer3D::ReserveMeshes(const size_t count)
	{
		_meshes.reserve(count);
	}

	void Renderer3D::SubmitMesh(const glm::mat4& transform, Submesh& submesh, MeshComponent::CullModeType cullMode)
	{
		DS_PROFILE_SCOPE()

		_meshes.emplace_back(transform, submesh, cullMode);
	}

	void Renderer3D::Flush(const Ref<RenderGraphData>& renderGraphData)
	{
		DS_PROFILE_SCOPE()		
		
		ShadowMapPass();
		RenderPass(renderGraphData->RenderPassTarget);
		LightingPass(renderGraphData);
		BloomPass(renderGraphData);
		FXAAPass(renderGraphData);
		CompositePass(renderGraphData);
		_meshes.clear();
	}

	void Renderer3D::ResetStats()
	{
		DS_PROFILE_SCOPE()		

		memset(&_stats, 0, sizeof(Statistics));
	}

	Renderer3D::Statistics Renderer3D::GetStats()
	{
		
		DS_PROFILE_SCOPE()		

		return _stats;
	}

	void Renderer3D::SetupCameraData(const CameraData& cameraData)
	{
		DS_PROFILE_SCOPE()		

		_ubCamera->Bind();
		_ubCamera->SetData(&cameraData, 0, sizeof(CameraData));
	}

	void Renderer3D::SetupLightsData()
	{
		DS_PROFILE_SCOPE()

		{ // Point light data setting
			struct PointLightData
			{
				glm::vec4 Position;
				glm::vec4 Color;
				glm::vec4 AttenFactors;
				glm::vec4 LightDir;
			};

			uint32_t numLights = 0;
			constexpr uint32_t size = sizeof(PointLightData);

			_ubPointLights->Bind();

			for(Entity e : _sceneLights)
			{
				const LightComponent& lightComponent = e.GetComponent<LightComponent>();
				if(lightComponent.Type == LightComponent::LightType::Directional)
					continue;

				glm::mat4 worldTransform = e.GetWorldTransform();

				glm::vec4 attenFactors = glm::vec4(
					lightComponent.Range,
					glm::cos(lightComponent.CutOffAngle),
					glm::cos(lightComponent.OuterCutOffAngle),
					static_cast<uint32_t>(lightComponent.Type)
				);
				glm::vec4 zDir = worldTransform * glm::vec4(0, 0, 1, 0);

				PointLightData pointLightData = {
					worldTransform[3],
					glm::vec4(lightComponent.Color, lightComponent.Intensity),
					attenFactors,
					zDir
				};

				_ubPointLights->SetData(&pointLightData, size * numLights, size);

				numLights++;
			}

			{
				struct DirectionalLightData
				{
					glm::vec4 Position;
					glm::vec4 Color;
					glm::vec4 LightDir;
					glm::mat4 DirLightViewProj;
				};

				uint32_t numLights = 0;
				constexpr uint32_t size = sizeof(DirectionalLightData);

				_ubDirectionalLights->Bind();

				for (Entity e : _sceneLights)
				{
					const LightComponent& lightComponent = e.GetComponent<LightComponent>();
					if (lightComponent.Type != LightComponent::LightType::Directional)
						continue;

					glm::mat4 worldTransform = e.GetWorldTransform();
				
					// Based off of +Z direction
					glm::vec4 zDir = worldTransform * glm::vec4(0, 0, 1, 0);
					float near_plane = -100.0f, far_plane = 100.0f;
					glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
					glm::vec3 pos = worldTransform[3];
					glm::vec3 dir = glm::normalize(glm::vec3(zDir));
					glm::vec3 lookAt = pos + dir;
					glm::mat4 dirLightView = glm::lookAt(pos, lookAt, glm::vec3(0, 1 ,0));
					glm::mat4 dirLightViewProj = lightProjection * dirLightView;

					DirectionalLightData dirLightData = 
					{
						glm::vec4(pos, static_cast<uint32_t>(lightComponent.ShadowQuality)),
						glm::vec4(lightComponent.Color, lightComponent.Intensity),
						zDir,
						dirLightViewProj
					};

					_ubDirectionalLights->SetData(&dirLightData, size * numLights, size);

					numLights++;
				}

				// Pass number of lights within the scene
				_ubDirectionalLights->SetData(&numLights, MAX_NUM_DIR_LIGHTS * size, sizeof(uint32_t));
			}
		}
	}

	void Renderer3D::FXAAPass(const Ref<RenderGraphData>& renderGraphData)
	{
		DS_PROFILE_SCOPE()		

		if(UseFXAA)
		{
			renderGraphData->FXAAPassTarget->Bind();
			_fxaaShader->Bind();
			_fxaaShader->SetFloat2("u_Threshold", FXAAThreshold);
			_fxaaShader->SetInt("u_Texture", 0);
			renderGraphData->LightingPassTarget->BindColorAttachment(0, 0);
			DrawQuad();
		}
	}

	void Renderer3D::CompositePass(const Ref<RenderGraphData>& renderGraphData)
	{
		DS_PROFILE_SCOPE()

		renderGraphData->CompositePassTarget->Bind();
		_hdrShader->Bind();
		const glm::vec4 tonemappingParams = { static_cast<int>(Tonemapping), Exposure, 0.0f, 0.0f };
		
		_hdrShader->SetFloat4("u_TonemappParams", tonemappingParams);
		_hdrShader->SetFloat("u_BloomStrength", UseBloom ? BloomStrength : -1.0f);
		_hdrShader->SetInt("u_Texture", 0);
		_hdrShader->SetInt("u_BloomTexture", 1);
		_hdrShader->SetInt("u_VignetteMask", 2);
		
		if (UseFXAA)
			renderGraphData->FXAAPassTarget->BindColorAttachment(0, 0);
		else
			renderGraphData->LightingPassTarget->BindColorAttachment(0, 0);

		if (UseBloom)
			renderGraphData->UpsampledFramebuffers[0]->BindColorAttachment(0, 1);

		if (VignetteOffset.a > 0.0f)
		{
			_hdrShader->SetFloat4("u_VignetteColor", VignetteColor);
			if (VignetteMask)
				VignetteMask->Bind(2);
		}
		_hdrShader->SetFloat4("u_VignetteOffset", VignetteOffset);

		DrawQuad();
	}

	void Renderer3D::BloomPass(const Ref<RenderGraphData>& renderGraphData)
	{
		DS_PROFILE_SCOPE()

			if (!UseBloom)
				return;

		const glm::vec4 threshold = { BloomThreshold, BloomKnee, BloomKnee * 2.0f, 0.25f / BloomKnee };
		glm::vec4 params = { BloomClamp, 2.0f, 0.0f, 0.0f };

		{
			DS_PROFILE_SCOPE("Prefilter")

			renderGraphData->PrefilteredFramebuffer->Bind();
			_bloomShader->Bind();
			_bloomShader->SetFloat4("u_Threshold", threshold);
			_bloomShader->SetFloat4("u_Params", params);
			_bloomShader->SetInt("u_Texture", 0);
			renderGraphData->LightingPassTarget->BindColorAttachment(0, 0);
			DrawQuad();
		}

		const size_t blurSamples = renderGraphData->BlurSamples;
		FramebufferSpecification spec = renderGraphData->PrefilteredFramebuffer->GetSpecification();
		{
			DS_PROFILE_SCOPE("Downsample")

			_gaussianBlurShader->Bind();
			_gaussianBlurShader->SetInt("u_Texture", 0);
			for (size_t i = 0; i < blurSamples; i++)
			{
				renderGraphData->TempBlurFramebuffers[i]->Bind();
				_gaussianBlurShader->SetInt("u_Horizontal", static_cast<int>(true));
				if (i == 0)
					renderGraphData->PrefilteredFramebuffer->BindColorAttachment(0, 0);
				else
					renderGraphData->DownsampledFramebuffers[i - 1]->BindColorAttachment(0, 0);
				DrawQuad();

				renderGraphData->DownsampledFramebuffers[i]->Bind();
				_gaussianBlurShader->SetInt("u_Horizontal", static_cast<int>(false));
				renderGraphData->TempBlurFramebuffers[i]->BindColorAttachment(0, 0);
				DrawQuad();
			}
		}
		
		{
			DS_PROFILE_SCOPE("Upsample")

			_bloomShader->Bind();
			_bloomShader->SetFloat4("u_Threshold", threshold);
			params = glm::vec4(BloomClamp, 3.0f, 1.0f, 1.0f);
			_bloomShader->SetFloat4("u_Params", params);
			_bloomShader->SetInt("u_Texture", 0);
			_bloomShader->SetInt("u_AdditiveTexture", 1);
			const size_t upsampleCount = blurSamples - 1;
			for (size_t i = upsampleCount; i > 0; i--)
			{
				renderGraphData->UpsampledFramebuffers[i]->Bind();
			
				if (i == upsampleCount)
				{
					renderGraphData->DownsampledFramebuffers[upsampleCount]->BindColorAttachment(0, 0);
					renderGraphData->DownsampledFramebuffers[upsampleCount - 1]->BindColorAttachment(0, 1);
				}
				else
				{
					renderGraphData->DownsampledFramebuffers[i]->BindColorAttachment(0, 0);
					renderGraphData->UpsampledFramebuffers[i + 1]->BindColorAttachment(0, 1);
				}
				DrawQuad();
			}

			renderGraphData->UpsampledFramebuffers[0]->Bind();
			params = glm::vec4(BloomClamp, 3.0f, 1.0f, 0.0f);
			_bloomShader->SetFloat4("u_Params", params);
			renderGraphData->UpsampledFramebuffers[1]->BindColorAttachment(0, 0);
			DrawQuad();
		}
	}

	void Renderer3D::LightingPass(const Ref<RenderGraphData>& renderGraphData)
	{
		DS_PROFILE_SCOPE()

		renderGraphData->LightingPassTarget->Bind();
		RenderCommand::Clear();

		_lightingShader->Bind();

		_lightingShader->SetInt("u_Albedo", 0);
		_lightingShader->SetInt("u_Normal", 1);
		_lightingShader->SetInt("u_MetallicRoughnessAO", 2);
		_lightingShader->SetInt("u_Emission", 3);
		_lightingShader->SetInt("u_Depth", 4);

		_lightingShader->SetInt("u_IrradianceMap", 5);
		_lightingShader->SetInt("u_RadianceMap", 6);
		_lightingShader->SetInt("u_BRDFLutMap", 7);

		int32_t samplers[MAX_NUM_DIR_LIGHTS];
		for (uint32_t i = 0; i < MAX_NUM_DIR_LIGHTS; i++)
			samplers[i] = static_cast<int32_t>(i + 8);
		_lightingShader->SetIntArray("u_DirectionalShadowMap", samplers, MAX_NUM_DIR_LIGHTS);

		renderGraphData->RenderPassTarget->BindColorAttachment(0, 0);
		renderGraphData->RenderPassTarget->BindColorAttachment(1, 1);
		renderGraphData->RenderPassTarget->BindColorAttachment(2, 2);
		renderGraphData->RenderPassTarget->BindColorAttachment(3, 3);
		renderGraphData->RenderPassTarget->BindDepthAttachment(4);
		
		if (_skylight)
		{
			const SkyLightComponent& skylightComponent = _skylight.GetComponent<SkyLightComponent>();
			if (skylightComponent.Texture)
			{
				_lightingShader->SetFloat("u_IrradianceIntensity", skylightComponent.Intensity);
				_lightingShader->SetFloat("u_EnvironmentRotation", skylightComponent.Rotation);
				skylightComponent.Texture->BindIrradianceMap(5);
				skylightComponent.Texture->BindRadianceMap(6);
			}
		}
		_BRDFLutTexture->Bind(7);
		
		uint32_t dirLightIndex = 0;
		for (const auto& lightEntity : _sceneLights)
		{
			const LightComponent& light = lightEntity.GetComponent<LightComponent>();
			if (light.Type == LightComponent::LightType::Directional)
			{
				if (dirLightIndex < MAX_NUM_DIR_LIGHTS)
				{
					light.ShadowMapFramebuffer->BindDepthAttachment(8 + dirLightIndex);
					dirLightIndex++;
				}
				else
				{
					break;
				}
			}
		}
		
		DrawQuad();
	}

	void Renderer3D::RenderPass(const Ref<Framebuffer>& renderTarget)
	{
		DS_PROFILE_SCOPE()

		renderTarget->Bind();
		RenderCommand::SetBlendState(false);
		RenderCommand::SetClearColor(glm::vec4(0.0f));
		RenderCommand::Clear();

		{
			DS_PROFILE_SCOPE("Skylight")

			if (_skylight)
			{
				const auto& skylightComponent = _skylight.GetComponent<SkyLightComponent>();
				if (skylightComponent.Texture)
				{
					RenderCommand::SetDepthMask(false);

					skylightComponent.Texture->Bind(0);
					_cubemapShader->Bind();
					_cubemapShader->SetFloat("u_Intensity", skylightComponent.Intensity);
					_cubemapShader->SetFloat("u_Rotation", skylightComponent.Rotation);

					DrawCube();

					RenderCommand::SetDepthMask(true);
				}
			}
		}
		
		{
			DS_PROFILE_SCOPE("Draw Meshes")

			MeshComponent::CullModeType currentCullMode = MeshComponent::CullModeType::Unknown;
			for (const auto& meshData : _meshes)
			{
				meshData.SubmeshGeometry.Mat->Bind();
				_shader->SetMat4("u_Model", meshData.Transform);
				
				if (currentCullMode != meshData.CullMode)
				{
					currentCullMode = meshData.CullMode;
					switch (currentCullMode)
					{
						case MeshComponent::CullModeType::DoubleSided:
							RenderCommand::DisableCulling();
							break;
						case MeshComponent::CullModeType::Front:
							RenderCommand::EnableCulling();
							RenderCommand::FrontCull();
							break;
						case MeshComponent::CullModeType::Back:
							RenderCommand::EnableCulling();
							RenderCommand::BackCull();
							break;
						default:
							DS_CORE_ASSERT(false)
							break;
					}
				}
				
				RenderCommand::DrawIndexed(meshData.SubmeshGeometry.Geometry);
				_stats.DrawCalls++;
				_stats.IndexCount += meshData.SubmeshGeometry.Geometry->GetIndexBuffer()->GetCount();
			}
		}
	}

	void Renderer3D::ShadowMapPass()
	{
		DS_PROFILE_SCOPE()

		for (const auto& lightEntity : _sceneLights)
		{
			const LightComponent& light = lightEntity.GetComponent<LightComponent>();
			if (light.Type != LightComponent::LightType::Directional)
				continue;

			light.ShadowMapFramebuffer->Bind();
			RenderCommand::Clear();

			glm::mat4 transform = lightEntity.GetWorldTransform();
			constexpr float nearPlane = -100.0f;
			constexpr float farPlane = 100.0f;
			
			glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, nearPlane, farPlane);
			
			glm::vec4 zDir = transform * glm::vec4(0, 0, 1, 0);

			glm::vec3 pos = transform[3];
			glm::vec3 dir = glm::normalize(glm::vec3(zDir));
			glm::vec3 lookAt = pos + dir;
			glm::mat4 dirLightView = glm::lookAt(pos, lookAt, glm::vec3(0, 1 ,0));
			glm::mat4 dirLightViewProj = lightProjection * dirLightView;

			_shadowMapShader->Bind();
			_shadowMapShader->SetMat4("u_ViewProjection", dirLightViewProj);

			for (auto it = _meshes.rbegin(); it != _meshes.rend(); ++it)
			{
				const MeshData& meshData = *it;
				_shadowMapShader->SetMat4("u_Model", meshData.Transform);
				RenderCommand::DrawIndexed(meshData.SubmeshGeometry.Geometry);
			}
		}
	}

} // namespace DawnStar
