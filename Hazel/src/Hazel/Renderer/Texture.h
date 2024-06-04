#pragma once
//纹理
#include"Hazel/Core/Base.h"
#include <string>
namespace Hazel {

    enum class ImageFormat//图像通道
    {
        None = 0,
        R8,
        RGB8,
        RGBA8,
        RGBA32F
    };

    struct TextureSpecification//纹理规范（默认设置）
    {
        uint32_t Width = 1;
        uint32_t Height = 1;
        ImageFormat Format = ImageFormat::RGBA8;
        bool GenerateMips = true;
    };

    class Texture {
    public:
        virtual ~Texture() = default;

        virtual const TextureSpecification& GetSpecification() const = 0;//获取纹理规范

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetRendererID() const = 0;
        
        virtual const std::string& GetPath() const = 0;
        
        virtual void SetData(void* data, uint32_t size = 0) = 0;
        
        virtual void Bind(uint32_t slot = 0) const = 0;

        virtual bool IsLoaded() const = 0;//是否已实际加载

        virtual bool operator== (const Texture& other)const = 0;

    };
    class Texture2D : public Texture {
    public:
        static Ref<Texture2D> Create(const std::string& path);
        static Ref<Texture2D> Create(const TextureSpecification& specification);////传入纹理规范
    };
}