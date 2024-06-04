#pragma once
//����
#include <stdint.h>
#include <cstring>

namespace Hazel {

    // ��������ԭʼ�������ȼ�
    struct Buffer
    {
        //�����С && ��ȡ��С
        uint8_t* Data = nullptr;//uint8_t����
        uint64_t Size = 0;

        Buffer() = default;

        Buffer(uint64_t size)
        {
            Allocate(size);
        }

        Buffer(const Buffer&) = default;

        static Buffer Copy(Buffer other)//����
        {
            Buffer result(other.Size);//����buffer�����
            memcpy(result.Data, other.Data, other.Size);//���ֽڷ���ֵ
            return result;//��������µĶ���
        }

        void Allocate(uint64_t size)//����ֵ
        {
            Release();

            Data = new uint8_t[size];
            Size = size;
        }

        void Release()//����
        {
            delete[] Data;
            Data = nullptr;
            Size = 0;
        }

        template<typename T>
        T* As()
        {
            return (T*)Data;//������ת��Ϊģ������
        }

        operator bool() const
        {
            return (bool)Data;
        }

    };

    struct ScopedBuffer//���巶Χ
    {
        ScopedBuffer(Buffer buffer)
            : m_Buffer(buffer)
        {
        }

        ScopedBuffer(uint64_t size)
            : m_Buffer(size)
        {
        }

        ~ScopedBuffer()
        {
            m_Buffer.Release();
        }

        uint8_t* Data() { return m_Buffer.Data; }
        uint64_t Size() { return m_Buffer.Size; }

        template<typename T>
        T* As()
        {
            return m_Buffer.As<T>();
        }

        operator bool() const { return m_Buffer; }
    private:
        Buffer m_Buffer;//����
    };


}