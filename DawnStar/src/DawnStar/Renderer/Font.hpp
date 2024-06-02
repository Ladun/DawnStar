#pragma once

#include <msdf-atlas-gen.h>
#include <DawnStar/Renderer/Texture.hpp>

namespace DawnStar
{
    struct MSDFData
    {
		std::vector<msdf_atlas::GlyphGeometry> Glyphs;
		msdf_atlas::FontGeometry FontGeometry;
    };

    class Font
	{
	public:
		Font(const std::filesystem::path& filepath);
		~Font();

		const MSDFData* GetMSDFData() const { return _data; }
		Ref<Texture2D> GetAtlasTexture() const { return _atlasTexture; }

		static Ref<Font> GetDefault();
	private:
		MSDFData* _data;
		Ref<Texture2D> _atlasTexture;
	};
}