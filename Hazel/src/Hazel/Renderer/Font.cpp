#include "hzpch.h"
#include "Font.h"

#undef INFINITE
#include "msdf-atlas-gen.h"

namespace Hazel {

    Font::Font(const std::filesystem::path& filepath)//msdfgen��
    {
        msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();//��ʼ��Freetype��
        if (ft)
        {
            std::string fileString = filepath.string();//���������ļ�Ϊmsdfgen::FontHandle*
            msdfgen::FontHandle* font = msdfgen::loadFont(ft, fileString.c_str());
            if (font)
            {
                msdfgen::Shape shape;
                if (msdfgen::loadGlyph(shape, font, 'C'))//�ַ�'C'��������Ϣ
                {
                    shape.normalize();//��һ��
                    //                      max. angle
                    msdfgen::edgeColoringSimple(shape, 3.0);//��Ե��ɫ
                    //           image width, height
                    msdfgen::Bitmap<float, 3> msdf(32, 32);//����һ��32x32��λͼ
                    //                     range, scale, translation
                    msdfgen::generateMSDF(msdf, shape, 4.0, 1.0, msdfgen::Vector2(4.0, 4.0));//����MSDFͼ��
                    msdfgen::savePng(msdf, "output.png");//����ΪΪPNG��ʽ
                }
                msdfgen::destroyFont(font);//�����������
            }
            msdfgen::deinitializeFreetype(ft);//����ʼ��Freetype��
        }
    }

}