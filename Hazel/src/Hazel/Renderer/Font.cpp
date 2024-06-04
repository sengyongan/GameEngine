	#include "hzpch.h"
#include "Font.h"

#undef INFINITE
#include "msdf-atlas-gen.h"
#include "FontGeometry.h"
#include "GlyphGeometry.h"

#include "MSDFData.h"

namespace Hazel {

    //创建和缓存 使用msdfgen库生成字形纹理图集，存储到Texture2D对象 (字体名称,字体大小,图案几何信息,字体几何信息,宽度,高度）
    template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
    static Ref<Texture2D> CreateAndCacheAtlas(const std::string& fontName, 
        float fontSize, const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
        const msdf_atlas::FontGeometry& fontGeometry, uint32_t width, uint32_t height)
    {
        //设置生成器属性
        msdf_atlas::GeneratorAttributes attributes;
        attributes.config.overlapSupport = true;
        attributes.scanlinePass = true;
        //即时图集生成器
        msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(width, height);
        generator.setAttributes(attributes);
        generator.setThreadCount(8);
        generator.generate(glyphs.data(), (int)glyphs.size());
        //生成字形纹理图集
        msdfgen::BitmapConstRef<T, N> bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();
        //纹理规范纹理
        TextureSpecification spec;
        spec.Width = bitmap.width;
        spec.Height = bitmap.height;
        spec.Format = ImageFormat::RGB8;
        spec.GenerateMips = false;
        //创建一个Texture2D对象，位图数据设置到该对象
        Ref<Texture2D> texture = Texture2D::Create(spec);
        texture->SetData((void*)bitmap.pixels, bitmap.width * bitmap.height * 3);
        return texture;
    }

    Font::Font(const std::filesystem::path& filepath)
        : m_Data(new MSDFData())
    {
        msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();//初始化Freetype库
        HZ_CORE_ASSERT(ft);

        std::string fileString = filepath.string();

        // TODO(Yan): msdfgen::loadFontData loads from memory buffer which we'll need 
        msdfgen::FontHandle* font = msdfgen::loadFont(ft, fileString.c_str());//字体路径转化未msdfgen::FontHandle
        if (!font)
        {
            HZ_CORE_ERROR("Failed to load font: {}", fileString);
            return;
        }
        //字符集范围
        struct CharsetRange
        {
            uint32_t Begin, End;
        };

        // From imgui_draw.cpp
        static const CharsetRange charsetRanges[] =
        {
            { 0x0020, 0x00FF }
        };

        msdf_atlas::Charset charset;
        for (CharsetRange range : charsetRanges)//字符集范围
        {
            for (uint32_t c = range.Begin; c <= range.End; c++)
                charset.add(c);
        }
        //设置字形缩放比例和em大小
        double fontScale = 1.0;
        m_Data->FontGeometry = msdf_atlas::FontGeometry(&m_Data->Glyphs);
        int glyphsLoaded = m_Data->FontGeometry.loadCharset(font, fontScale, charset);//加载字符集到FontGeometry对象中
        HZ_CORE_INFO("Loaded {} glyphs from font (out of {})", glyphsLoaded, charset.size());

        double emSize = 40.0;//fontSize

        //字形打包        //设置打包器的参数
        msdf_atlas::TightAtlasPacker atlasPacker;
        // atlasPacker.setDimensionsConstraint()
        atlasPacker.setPixelRange(2.0);
        atlasPacker.setMiterLimit(1.0);
        atlasPacker.setPadding(0);
        atlasPacker.setScale(emSize);
        int remaining = atlasPacker.pack(m_Data->Glyphs.data(), (int)m_Data->Glyphs.size());//将字形打包到纹理图中
        HZ_CORE_ASSERT(remaining == 0);

        //获取纹理图的宽度和高度以及缩放比例
        int width, height;
        atlasPacker.getDimensions(width, height);
        emSize = atlasPacker.getScale();

        ///使用MSDF///对字形进行边缘着色//////////////////////////////////////////
        #define DEFAULT_ANGLE_THRESHOLD 3.0
        #define LCG_MULTIPLIER 6364136223846793005ull
        #define LCG_INCREMENT 1442695040888963407ull
        #define THREAD_COUNT 8
        // if MSDF || MTSDF

        uint64_t coloringSeed = 0;
        bool expensiveColoring = false;
        if (expensiveColoring)
        {
            msdf_atlas::Workload([&glyphs = m_Data->Glyphs, &coloringSeed](int i, int threadNo) -> bool {
                unsigned long long glyphSeed = (LCG_MULTIPLIER * (coloringSeed ^ i) + LCG_INCREMENT) * !!coloringSeed;
                glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
                return true;
                }, m_Data->Glyphs.size()).finish(THREAD_COUNT);
        }
        else {
            unsigned long long glyphSeed = coloringSeed;
            for (msdf_atlas::GlyphGeometry& glyph : m_Data->Glyphs)
            {
                glyphSeed *= LCG_MULTIPLIER;
                glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
            }
        }

        //创建并缓存纹理图,存储到m_AtlasTexture
        m_AtlasTexture = CreateAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>("Test",
            (float)emSize, m_Data->Glyphs, m_Data->FontGeometry, width, height);

#if 0
        msdfgen::Shape shape;
        if (msdfgen::loadGlyph(shape, font, 'C'))
        {
            shape.normalize();
            //                      max. angle
            msdfgen::edgeColoringSimple(shape, 3.0);
            //           image width, height
            msdfgen::Bitmap<float, 3> msdf(32, 32);
            //                     range, scale, translation
            msdfgen::generateMSDF(msdf, shape, 4.0, 1.0, msdfgen::Vector2(4.0, 4.0));
            msdfgen::savePng(msdf, "output.png");
        }
#endif

        msdfgen::destroyFont(font);
        msdfgen::deinitializeFreetype(ft);
    }

    Font::~Font()
    {
        delete m_Data;
    }


    Ref<Font> Font::GetDefault()
    {
        static Ref<Font> DefaultFont;
        if (!DefaultFont)
            DefaultFont = CreateRef<Font>("assets/fonts/opensans/OpenSans-Regular.ttf");

        return DefaultFont;
    }

}