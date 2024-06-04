#include "hzpch.h"
#include "FileSystem.h"

namespace Hazel {

    Buffer FileSystem::ReadFileBinary(const std::filesystem::path& filepath)
    {
        std::ifstream stream(filepath, std::ios::binary | std::ios::ate);//创建流，以二进制模式和文件末尾位置打开文件

        if (!stream)
        {
            // Failed to open the file
            return {};
        }

        std::streampos end = stream.tellg();//获取文件末尾的位置，将其存储在end变量中
        stream.seekg(0, std::ios::beg);//将文件指针移回文件开头
        uint32_t size = end - stream.tellg();//计算文件大小，即end - 当前文件指针位置

        if (size == 0)//文件为空
        {
            // File is empty
            return {};
        }

        Buffer buffer(size);//字符数组buffer，大小为文件大小
        stream.read(buffer.As<char>(), size);//将文件内容读取到buffer中
        stream.close();//关闭文件流

        return buffer;//返回包含文件内容的buffer

    }

}