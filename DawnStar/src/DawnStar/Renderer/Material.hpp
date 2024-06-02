#pragma once

namespace DawnStar
{
	class Texture2D;
	class Shader;

	using MaterialData = void*;
	class Material
	{

    public:
		explicit Material(const std::filesystem::path& shaderPath = "assets/shaders/PBR.glsl");
		virtual ~Material();

		Material(const Material& other) = default;
		Material(Material&& other) = default;

		void Invalidate();
		void Bind() const;
		void Unbind() const;
		Ref<Shader> GetShader() const;
		Ref<Texture2D> GetTexture(uint32_t slot);
		void SetTexture(uint32_t slot, const Ref<Texture2D>& texture);

		template<typename T>
		T GetData(const std::string& name) const
		{
			MaterialData value = GetDataInternal(name);
			return *static_cast<T*>(value);
		}

		template<typename T>
		void SetData(const std::string& name, T data) const
		{
			SetDataInternal(name, &data);
		}

	private:
		MaterialData GetDataInternal(const std::string& name) const;
		void SetDataInternal(const std::string& name, MaterialData data) const;

	private:
		Ref<Shader> _shader = nullptr;
		char*       _buffer = nullptr;
		size_t      _bufferSizeInBytes = 0;

		std::unordered_map<uint32_t, Ref<Texture2D>> _textures;

		static Ref<Texture2D> _whiteTexture;
    };
} // namespace DawnStar
