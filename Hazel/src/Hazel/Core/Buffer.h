#pragma once
//缓冲
#include <stdint.h>
#include <cstring>

namespace Hazel {

    // 非所有者原始缓冲区等级
    struct Buffer
    {
        //分配大小 && 获取大小
        uint8_t* Data = nullptr;//uint8_t数组
        uint64_t Size = 0;

        Buffer() = default;

        Buffer(uint64_t size)
        {
            Allocate(size);
        }

        Buffer(const Buffer&) = default;

        static Buffer Copy(Buffer other)//复制
        {
            Buffer result(other.Size);//创建buffer类对象
            memcpy(result.Data, other.Data, other.Size);//按字节分配值
            return result;//返回这个新的对象
        }

        void Allocate(uint64_t size)//分配值
        {
            Release();

            Data = new uint8_t[size];
            Size = size;
        }

        void Release()//重置
        {
            delete[] Data;
            Data = nullptr;
            Size = 0;
        }

        template<typename T>
        T* As()
        {
            return (T*)Data;//将数据转化为模板类型
        }

        operator bool() const
        {
            return (bool)Data;
        }

    };

    struct ScopedBuffer//缓冲范围
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
        Buffer m_Buffer;//缓冲
    };


}