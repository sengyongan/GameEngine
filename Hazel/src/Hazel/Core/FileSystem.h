#pragma once
//�ļ�ϵͳ
#include "Hazel/Core/Buffer.h"

namespace Hazel {

    class FileSystem
    {
    public:
        // TODO: move to FileSystem class
        static Buffer ReadFileBinary(const std::filesystem::path& filepath);//��ȡ�������ļ�,���ػ���
    };

}