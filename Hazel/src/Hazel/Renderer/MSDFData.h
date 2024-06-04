#pragma once
//实时字体渲染的数据
#include <vector>

#undef INFINITE
#include "msdf-atlas-gen.h"

namespace Hazel {

    struct MSDFData
    {
        std::vector<msdf_atlas::GlyphGeometry> Glyphs;
        msdf_atlas::FontGeometry FontGeometry;
    };


}