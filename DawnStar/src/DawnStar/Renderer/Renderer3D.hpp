#pragma once

#include <DawnStar/Scene/Components.hpp>

namespace DawnStar
{
	class Shader;
	class ShaderLibrary;
	class UniformBuffer;
	struct Submesh;
	struct RenderGraphData;
	class Entity;
	struct CameraData;

    class Renderer3D
    {
    public:
        static constexpr uint32_t MAX_NUM_LIGHTS = 200;
        static constexpr uint32_t MAX_NUM_DIR_LIGHTS = 3;

        static void Init();
        static void Shutdown();

		static void BeginScene(const CameraData& cameraData, Entity cubemap, std::vector<Entity>&& lights);
		static void EndScene(const Ref<RenderGraphData>& renderTarget);

        static void DrawCube();
		static void DrawQuad();
		static void ReserveMeshes(size_t count);
		static void SubmitMesh(const glm::mat4& transform, Submesh& submesh, MeshComponent::CullModeType cullMode);

        static ShaderLibrary& GetShaderLibrary() { return _shaderLibrary; }

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t IndexCount = 0;
		};

		static void ResetStats();
		static Statistics GetStats();
        
	private:
		static void SetupCameraData(const CameraData& cameraData);
		static void SetupLightsData();
		static void Flush(const Ref<RenderGraphData>& renderGraphData);
		static void FXAAPass(const Ref<RenderGraphData>& renderGraphData);
		static void CompositePass(const Ref<RenderGraphData>& renderGraphData);
		static void BloomPass(const Ref<RenderGraphData>& renderGraphData);
		static void LightingPass(const Ref<RenderGraphData>& renderGraphData);
		static void RenderPass(const Ref<Framebuffer>& renderTarget);
		static void ShadowMapPass();

	private:
		struct MeshData
		{
			glm::mat4 Transform;
			Submesh& SubmeshGeometry;
			MeshComponent::CullModeType CullMode;

			MeshData(const glm::mat4& transform, Submesh& submesh, const MeshComponent::CullModeType cullMode)
				: Transform(transform), SubmeshGeometry(submesh), CullMode(cullMode)
			{
			}
		};
		static Statistics _stats;
		static ShaderLibrary _shaderLibrary;
		static std::vector<Renderer3D::MeshData> _meshes;
		static Ref<Texture2D> _BRDFLutTexture;
		static Ref<Shader> _shader;
		static Ref<Shader> _lightingShader;
		static Ref<Shader> _shadowMapShader;
		static Ref<Shader> _cubemapShader;
		static Ref<Shader> _gaussianBlurShader;
		static Ref<Shader> _fxaaShader;
		static Ref<Shader> _hdrShader;
		static Ref<Shader> _bloomShader;
		static Ref<VertexArray> _quadVertexArray;
		static Ref<VertexArray> _cubeVertexArray;
		static Ref<UniformBuffer> _ubCamera;
		static Ref<UniformBuffer> _ubPointLights;
		static Ref<UniformBuffer> _ubDirectionalLights;

		static Entity _skylight;
		static std::vector<Entity> _sceneLights;

	public:

		enum class TonemappingType { None = 0, ACES, Filmic, Uncharted };

		static TonemappingType Tonemapping;
		static float Exposure;
		static bool UseBloom;
		static float BloomStrength;
		static float BloomThreshold;
		static float BloomKnee;
		static float BloomClamp;
		static bool UseFXAA;
		static glm::vec2 FXAAThreshold;			// x: current threshold, y: relative threshold
		static glm::vec4 VignetteColor;			// rgb: color, a: intensity
		static glm::vec4 VignetteOffset;		// xy: offset, z: useMask, w: enable/disable effect
		static Ref<Texture2D> VignetteMask;
    };
} // namespace DawnStar
