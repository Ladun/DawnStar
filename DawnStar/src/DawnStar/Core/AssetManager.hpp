#pragma once

namespace DawnStar
{
	class Mesh;
	class TextureCubemap;
	class Texture2D;

	class AssetManager
	{
	public:
		static void Init();
		static void Shutdown();

		static Ref<Texture2D>& GetTexture2D(const std::string& path);
		static Ref<TextureCubemap>& GetTextureCubemap(const std::string& path);
		static Ref<Mesh>& GetMesh(const std::string& path);
	};
}