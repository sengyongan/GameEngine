#pragma once

namespace Hazel {

    class Font
    {
    public:
        Font(const std::filesystem::path& font);//构造函数：加载字体文件并生成一个字符的轮廓图像
    };

}