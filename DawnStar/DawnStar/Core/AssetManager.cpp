#include <dspch.hpp>
#include <DawnStar/Core/AssetManager.hpp>

#include <stb_image.h>

#include <DawnStar/Core/Application.hpp>
#include <DawnStar/Renderer/Mesh.hpp>
#include <DawnStar/Renderer/Texture.hpp>
#include <DawnStar/Utils/StringUtils.hpp>


namespace DawnStar
{    
	inline static std::unordered_map<std::string, Ref<Texture2D>, UM_StringTransparentEquality> _texture2DMap;
	inline static std::unordered_map<std::string, Ref<TextureCubemap>, UM_StringTransparentEquality> _textureCubeMap;
	inline static std::unordered_map<std::string, Ref<Mesh>, UM_StringTransparentEquality> _meshMap;
	inline static std::vector<std::future<void>> _futures;

    void AssetManager::Init()
    {
        DS_PROFILE_SCOPE()

    }

    void AssetManager::Shutdown()
    {
        DS_PROFILE_SCOPE()

        _texture2DMap.clear();
        _textureCubeMap.clear();
        _meshMap.clear();
        _futures.clear();
    }

    static void LoadTexture2D(Texture2D* tex, const std::string_view path)
    {
        DS_PROFILE_THREAD("IO Thread")

        stbi_set_flip_vertically_on_load(1);
        int width, height, channels;
		stbi_uc* data = nullptr;
		{
			DS_PROFILE_SCOPE("stbi_load Texture")

			data = stbi_load(path.data(), &width, &height, &channels, 0);
		}
		DS_CORE_ASSERT(data, "Failed to load image!")
		Application::Get().SubmitToMainThread([tex, path, width, height, data, channels]() { tex->Invalidate(path, width, height, data, channels); stbi_image_free(data); });
    }

    Ref<Texture2D>& AssetManager::GetTexture2D(const std::string& path)
    {
        DS_PROFILE_SCOPE()        

		const auto& it = _texture2DMap.find(path);
		if (it != _texture2DMap.end())
			return it->second;

		Ref<Texture2D> texture = CreateRef<Texture2D>();
		_texture2DMap.emplace(path, texture);
		_futures.push_back(std::async(std::launch::async, &LoadTexture2D, texture.get(), path));
		return _texture2DMap[path];
    }
    
	static void LoadTextureCubemap(TextureCubemap* tex, const std::string_view path)
	{
		DS_PROFILE_THREAD("IO Thread")

		stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		float* data = nullptr;
		{
			DS_PROFILE_SCOPE("stbi_load Texture")

			data = stbi_loadf(path.data(), &width, &height, &channels, 0);
		}
		DS_CORE_ASSERT(data, "Failed to load image!")
		Application::Get().SubmitToMainThread([tex, path, width, height, data, channels]() { tex->Invalidate(path, width, height, data, channels); stbi_image_free(data); });
	}

	Ref<TextureCubemap>& AssetManager::GetTextureCubemap(const std::string& path)
	{
		DS_PROFILE_SCOPE()

		const auto& it = _textureCubeMap.find(path);
		if (it != _textureCubeMap.end())
			return it->second;

		Ref<TextureCubemap> texture = CreateRef<TextureCubemap>();
		_textureCubeMap.emplace(path, texture);
		_futures.push_back(std::async(std::launch::async, &LoadTextureCubemap, texture.get(), path));
		return _textureCubeMap[path];
	}

	Ref<Mesh>& AssetManager::GetMesh(const std::string& path)
	{
		DS_PROFILE_SCOPE()

		const auto& it = _meshMap.find(path);
		if (it != _meshMap.end())
			return it->second;

		Ref<Mesh> mesh = CreateRef<Mesh>(path.c_str());
		_meshMap.emplace(path, mesh);
		return _meshMap[path];
	}
} // namespace DawnStar
