#include "hzpch.h"
#include "Font.h"

#undef INFINITE
#include "msdf-atlas-gen.h"

namespace Hazel {

    Font::Font(const std::filesystem::path& filepath)//msdfgen库
    {
        msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();//初始化Freetype库
        if (ft)
        {
            std::string fileString = filepath.string();//加载字体文件为msdfgen::FontHandle*
            msdfgen::FontHandle* font = msdfgen::loadFont(ft, fileString.c_str());
            if (font)
            {
                msdfgen::Shape shape;
                if (msdfgen::loadGlyph(shape, font, 'C'))//字符'C'的轮廓信息
                {
                    shape.normalize();//归一化
                    //                      max. angle
                    msdfgen::edgeColoringSimple(shape, 3.0);//边缘着色
                    //           image width, height
                    msdfgen::Bitmap<float, 3> msdf(32, 32);//创建一个32x32的位图
                    //                     range, scale, translation
                    msdfgen::generateMSDF(msdf, shape, 4.0, 1.0, msdfgen::Vector2(4.0, 4.0));//生成MSDF图像
                    msdfgen::savePng(msdf, "output.png");//保存为为PNG格式
                }
                msdfgen::destroyFont(font);//销毁字体对象
            }
            msdfgen::deinitializeFreetype(ft);//反初始化Freetype库
        }
    }

}