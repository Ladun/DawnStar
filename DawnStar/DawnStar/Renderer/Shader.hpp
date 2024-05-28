#pragma once

#include <DawnStar/Utils/StringUtils.hpp>

namespace DawnStar
{
	enum class MaterialPropertyType
	{
		None = 0,
		Sampler2D,
		Bool,
		Int,
		Float,
		Float2,
		Float3,
		Float4,
	};

	struct MaterialProperty
	{
		MaterialPropertyType Type;
		size_t SizeInBytes;
		size_t OffsetInBytes;

		std::string DisplayName;
		bool IsSlider;
		bool IsColor;
	};

	class Shader
	{
	public:
		Shader(const std::filesystem::path &filepath);
		~Shader();

		void Recompile(const std::filesystem::path& path);
		void Bind() const ;
		void Unbind() const;

		void SetInt(const std::string& name, int value);
		void SetIntArray(const std::string& name, const int* value, uint32_t count);
		void SetFloat(const std::string& name, float value);
		void SetFloat2(const std::string& name, const glm::vec2& value);
		void SetFloat3(const std::string& name, const glm::vec3& value);
		void SetFloat4(const std::string& name, const glm::vec4& value);
		void SetMat3(const std::string& name, const glm::mat3& value);
		void SetMat4(const std::string& name, const glm::mat4& value);
		void SetUniformBlock(const std::string& name, uint32_t blockIndex);

		const std::string& GetName() const { return _name; }

		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformIntArray(const std::string& name, const int* values, uint32_t count);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
		
		
		std::unordered_map<std::string, MaterialProperty, UM_StringTransparentEquality>& GetMaterialProperties() { return _materialProperties; }

		static constexpr size_t GetSizeInBytes(MaterialPropertyType type)
		{
			switch (type)
			{
				case MaterialPropertyType::None: return 0;
				case MaterialPropertyType::Sampler2D: [[fallthrough]];
				case MaterialPropertyType::Bool: [[fallthrough]];
				case MaterialPropertyType::Int: return sizeof(int32_t);
				case MaterialPropertyType::Float: return sizeof(float);
				case MaterialPropertyType::Float2: return sizeof(glm::vec2);
				case MaterialPropertyType::Float3: return sizeof(glm::vec3);
				case MaterialPropertyType::Float4: return sizeof(glm::vec4);
			}

			return 0;
		}

	private:
		int GetLocation(const std::string& name);
		std::unordered_map<uint32_t, std::string> Preprocess(const std::string& source);
		void Compile(const std::unordered_map<uint32_t, std::string>& shaderSources, const std::string& name);

	private:
		uint32_t _rendererID;
		std::string _name;
		
		std::unordered_map<std::string, int, UM_StringTransparentEquality> _uniformLocationCache;
		std::unordered_map<std::string, MaterialProperty, UM_StringTransparentEquality> _materialProperties;
	};

	class ShaderLibrary
	{
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> Load(const std::filesystem::path &filepath);
		void ReloadAll();

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name) const;

	private:
		std::unordered_map<std::string, Ref<Shader>, UM_StringTransparentEquality> _shaders;
		std::unordered_map<std::string, std::string, UM_StringTransparentEquality> _shaderPaths;
	};
}