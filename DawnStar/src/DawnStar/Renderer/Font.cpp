#include <dspch.hpp>
#include <DawnStar/Renderer/Font.hpp>

#include <FontGeometry.h>
#include <GlyphGeometry.h>

namespace DawnStar
{
    template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
    static Ref<Texture2D> CreateAndCacheAtlas(const std::string& fontName, 
                                              float fontSize, 
                                              const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
                                              const msdf_atlas::FontGeometry& fontGeometry,
                                              uint32_t width, uint32_t height)
    {
        msdf_atlas::GeneratorAttributes attribute;
        attribute.config.overlapSupport = true;
        attribute.scanlinePass = true;

        msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(width, height);
        generator.setAttributes(attribute);
        generator.setThreadCount(8);
        generator.generate(glyphs.data(), (int)glyphs.size());

        msdfgen::BitmapConstRef<T, N> bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();
#if 1        
        msdfgen::savePng(bitmap, "output.png");
#endif

        Ref<Texture2D> texture = CreateRef<Texture2D>(width, height);
        texture->SetData((void*)bitmap.pixels, bitmap.width * bitmap.height * 3);
        return texture;
    }

    Font::Font(const std::filesystem::path& filepath)
        : _data(new MSDFData())
    {
        DS_PROFILE_SCOPE()

        msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
        DS_CORE_ASSERT(ft);

		std::string fileString = filepath.string();

        msdfgen::FontHandle* font = msdfgen::loadFont(ft, fileString.c_str());
        if(!font)
        {
			DS_CORE_ERROR("Failed to load font: {}", fileString);
			return;
        }

        struct CharsetRange
        {
            uint32_t Begin, End;
        };

        static const CharsetRange charsetRanges[] =
		{
			{ 0x0020, 0x00FF },
            { 0xAC00, 0xD7A3 }
		};

        msdf_atlas::Charset charset;
		for (CharsetRange range : charsetRanges)
		{
			for (uint32_t c = range.Begin; c <= range.End; c++)
				charset.add(c);
		}

        double fontScale = 1.0;
		_data->FontGeometry = msdf_atlas::FontGeometry(&_data->Glyphs);
		int glyphsLoaded = _data->FontGeometry.loadCharset(font, fontScale, charset);
		DS_CORE_INFO("Loaded {0} glyphs from font ({2})(out of {1})", glyphsLoaded, charset.size(), filepath.string());

        double emSize = 40.0;

        msdf_atlas::TightAtlasPacker atlasPacker;
        atlasPacker.setPixelRange(2.0);
        atlasPacker.setMiterLimit(1.0);
        atlasPacker.setPadding(0);
        atlasPacker.setScale(emSize);
        int remaining = atlasPacker.pack(_data->Glyphs.data(), (int)_data->Glyphs.size());
        DS_CORE_ASSERT(remaining == 0);

        int width, height;
        atlasPacker.getDimensions(width, height);
        emSize = atlasPacker.getScale();
                
#define DEFAULT_ANGLE_THRESHOLD 3.0
#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull
#define THREAD_COUNT 8

        uint64_t coloringSeed = 0;
        bool expensiveColoring = false;
        if (expensiveColoring)
        {
            msdf_atlas::Workload([&glyphs = _data->Glyphs, &coloringSeed](int i, int threadNo) -> bool {
                unsigned long long glyphSeed = (LCG_MULTIPLIER * (coloringSeed ^ i) + LCG_INCREMENT) * !!coloringSeed;
                glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
                return true;
            }, _data->Glyphs.size()).finish(THREAD_COUNT);
        }
        else
        {
			unsigned long long glyphSeed = coloringSeed;
			for (msdf_atlas::GlyphGeometry& glyph : _data->Glyphs)
			{
				glyphSeed *= LCG_MULTIPLIER;
				glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
			}

        }

        _atlasTexture = CreateAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>("Test", (float)emSize, _data->Glyphs, _data->FontGeometry, width, height);

		msdfgen::destroyFont(font);
		msdfgen::deinitializeFreetype(ft);
    }

	Font::~Font()
	{
		delete _data;
	}


	Ref<Font> Font::GetDefault()
	{
		static Ref<Font> DefaultFont;
		if (!DefaultFont)
			DefaultFont = CreateRef<Font>("assets/fonts/OpenSans-Regular.ttf");

		return DefaultFont;
	}
}