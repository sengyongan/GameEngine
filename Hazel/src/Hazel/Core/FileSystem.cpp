#include "hzpch.h"
#include "FileSystem.h"

namespace Hazel {

    Buffer FileSystem::ReadFileBinary(const std::filesystem::path& filepath)
    {
        std::ifstream stream(filepath, std::ios::binary | std::ios::ate);//���������Զ�����ģʽ���ļ�ĩβλ�ô��ļ�

        if (!stream)
        {
            // Failed to open the file
            return {};
        }

        std::streampos end = stream.tellg();//��ȡ�ļ�ĩβ��λ�ã�����洢��end������
        stream.seekg(0, std::ios::beg);//���ļ�ָ���ƻ��ļ���ͷ
        uint32_t size = end - stream.tellg();//�����ļ���С����end - ��ǰ�ļ�ָ��λ��

        if (size == 0)//�ļ�Ϊ��
        {
            // File is empty
            return {};
        }

        Buffer buffer(size);//�ַ�����buffer����СΪ�ļ���С
        stream.read(buffer.As<char>(), size);//���ļ����ݶ�ȡ��buffer��
        stream.close();//�ر��ļ���

        return buffer;//���ذ����ļ����ݵ�buffer

    }

}