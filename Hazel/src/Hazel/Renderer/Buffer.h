#pragma once
//缓冲基类——布局，顶点，索引 缓冲
namespace Hazel {
    enum class ShaderDataType {//所有类型
        None = 0,Float, Float2, Float3, Float4,Mat3, Mat4,Int, Int2, Int3, Int4,Bool
    };
    //步长
    static uint32_t ShaderDataTypeSize(ShaderDataType type) {

        switch (type)
        {
        case ShaderDataType::Float:    return 4;
        case ShaderDataType::Float2:   return 4 * 2;
        case ShaderDataType::Float3:   return 4 * 3;
        case ShaderDataType::Float4:   return 4 * 4;
        case ShaderDataType::Mat3:     return 4 * 3 * 3;//36字节
        case ShaderDataType::Mat4:     return 4 * 4 * 4;
        case ShaderDataType::Int:      return 4;
        case ShaderDataType::Int2:     return 4 * 2;
        case ShaderDataType::Int3:     return 4 * 3;
        case ShaderDataType::Int4:     return 4 * 4;
        case ShaderDataType::Bool:     return 1;
        }
        HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }//(GLuint index, GLint size, GLenum type, GLboolean normalized,GLsizei stride, const void *pointer);
    struct ButterElements {
        std::string Name;
        ShaderDataType Type;//类型——enum
        uint32_t Size;//类型大小
        uint32_t Offest;//偏移量
        bool Normalized;//正规化
        ButterElements(){}
        ButterElements(ShaderDataType type, const std::string& name,bool normalized = false) ://构造
            Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offest(0) , Normalized(normalized){
        }
        uint32_t GetComponentCount() const//
        {
            switch (Type)
            {
            case ShaderDataType::Float:   return 1;
            case ShaderDataType::Float2:  return 2;
            case ShaderDataType::Float3:  return 3;
            case ShaderDataType::Float4:  return 4;
            case ShaderDataType::Mat3:    return 3 * 3;
            case ShaderDataType::Mat4:    return 4 * 4;
            case ShaderDataType::Int:     return 1;
            case ShaderDataType::Int2:    return 2;
            case ShaderDataType::Int3:    return 3;
            case ShaderDataType::Int4:    return 4;
            case ShaderDataType::Bool:    return 1;
            }

            HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
            return 0;
        }
    };
    class BufferLayout {//渲染布局
    public:
        BufferLayout(){}
        //initializer_list表示初始化列表。在构造函数内部遍历这个列表并输出其中的元素。
        BufferLayout(const std::initializer_list<ButterElements>& elements)
            :m_Elements(elements){
            CalculateOffsetsAndStride();//计算偏移和距离
        }
        inline uint32_t GetStride() const { return m_Stride; }
        inline const std::vector<ButterElements>& GetElements() const { return m_Elements; }
        std::vector<ButterElements>::iterator begin() { return m_Elements.begin(); }
        std::vector<ButterElements>::iterator end() { return m_Elements.end(); }
        std::vector<ButterElements>::const_iterator begin() const { return m_Elements.begin(); }//???
        std::vector<ButterElements>::const_iterator end() const { return m_Elements.end(); }
    private:
        void CalculateOffsetsAndStride() {
            uint32_t offset = 0;
            m_Stride = 0;
            for (auto& element : m_Elements) {//获取传入的这组值，每个值有（type，name）
                element.Offest = offset;//当前值，设置偏移位置
                offset += element.Size;//更新下一个值偏移
                m_Stride += element.Size;
            }
        }
    private:
        std::vector<ButterElements> m_Elements;//传递的ButterElements列表
        uint32_t m_Stride = 0;//步长
    };
 //  
    class VertexBuffer {
    public:
        virtual ~VertexBuffer() {}
        virtual void Bind() const= 0;//空的虚函数
        virtual void Unbind() const = 0;
        //
        virtual const BufferLayout& GetLayout() const = 0;//获取布局
        virtual void SetLayout(const BufferLayout& layout)=0;//设置布局
        //
        static Ref<VertexBuffer>  Create(float* vertices, uint32_t size);
    };
//
    class IndexBuffer {
    public:
        virtual ~IndexBuffer(){}
        virtual void Bind()const = 0;//空的虚函数
        virtual void Unbind() const = 0;

        virtual uint32_t GetCount()const = 0;//返回计数
        static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t size);
    };
}