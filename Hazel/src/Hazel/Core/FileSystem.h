#pragma once
//文件系统
#include "Hazel/Core/Buffer.h"

namespace Hazel {

    class FileSystem
    {
    public:
        // TODO: move to FileSystem class
        static Buffer ReadFileBinary(const std::filesystem::path& filepath);//读取二进制文件,返回缓冲
    };

}