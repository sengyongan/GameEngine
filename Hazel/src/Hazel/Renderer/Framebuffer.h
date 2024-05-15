#pragma once
//帧缓冲--图片
#include "Hazel/Core/Base.h"
namespace Hazel {
    enum class FramebufferTextureFormat//帧缓冲纹理格式
    {
        None = 0,

        // Color//颜色
        RGBA8,
        RED_INTEGER,//红色整形

        // Depth/stencil//深度/模板
        DEPTH24STENCIL8,

        // Defaults
        Depth = DEPTH24STENCIL8//depth的默认值
    };

    struct FramebufferTextureSpecification//帧缓冲纹理规范
    {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(FramebufferTextureFormat format)//格式
            : TextureFormat(format) {}

        FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;//纹理格式变量
        // TODO: filtering/wrap
    };

    struct FramebufferAttachmentSpecification//帧缓冲器附件规范
    {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)//纹理规范(接受列表）
            : Attachments(attachments) {}

        std::vector<FramebufferTextureSpecification> Attachments;//数组类型
    };

    struct FramebufferSpecification {//帧缓冲器规范
        uint32_t Width, Height;
        uint32_t Samples = 1;//多重采样
        FramebufferAttachmentSpecification Attachments;//附件规范-
        bool SwapChainTarget = false;//交换链目标
    };

    ///Framebuffer///////////////////////////////////////////////////////////////////////////////////////////////////////
    class Framebuffer {
    public:
        virtual ~Framebuffer() = default;
        virtual void Bind() = 0 ;
        virtual void Unbind() = 0;

        virtual void Resize(uint32_t Width,uint32_t  Height) = 0;
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;//读取一个像素的颜色值：读取xy鼠标位置的附件贴图索引的像素

        virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;//清除纹理图像数据（附件索引，清除为指定值）

        virtual uint32_t  GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

        virtual const FramebufferSpecification& GetSpecification() const = 0;
        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);

    };
}