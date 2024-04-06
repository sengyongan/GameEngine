#pragma once
//������ࡪ�����֣����㣬���� ����
namespace Hazel {
    enum class ShaderDataType {//��������
        None = 0,Float, Float2, Float3, Float4,Mat3, Mat4,Int, Int2, Int3, Int4,Bool
    };
    //����
    static uint32_t ShaderDataTypeSize(ShaderDataType type) {

        switch (type)
        {
        case ShaderDataType::Float:    return 4;
        case ShaderDataType::Float2:   return 4 * 2;
        case ShaderDataType::Float3:   return 4 * 3;
        case ShaderDataType::Float4:   return 4 * 4;
        case ShaderDataType::Mat3:     return 4 * 3 * 3;//36�ֽ�
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
        ShaderDataType Type;//���͡���enum
        uint32_t Size;//���ʹ�С
        uint32_t Offest;//ƫ����
        bool Normalized;//���滯
        ButterElements(){}
        ButterElements(ShaderDataType type, const std::string& name,bool normalized = false) ://����
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
    class BufferLayout {//��Ⱦ����
    public:
        BufferLayout(){}
        //initializer_list��ʾ��ʼ���б��ڹ��캯���ڲ���������б�������е�Ԫ�ء�
        BufferLayout(const std::initializer_list<ButterElements>& elements)
            :m_Elements(elements){
            CalculateOffsetsAndStride();//����ƫ�ƺ;���
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
            for (auto& element : m_Elements) {//��ȡ���������ֵ��ÿ��ֵ�У�type��name��
                element.Offest = offset;//��ǰֵ������ƫ��λ��
                offset += element.Size;//������һ��ֵƫ��
                m_Stride += element.Size;
            }
        }
    private:
        std::vector<ButterElements> m_Elements;//���ݵ�ButterElements�б�
        uint32_t m_Stride = 0;//����
    };
 //  
    class VertexBuffer {
    public:
        virtual ~VertexBuffer() {}
        virtual void Bind() const= 0;//�յ��麯��
        virtual void Unbind() const = 0;
        //
        virtual const BufferLayout& GetLayout() const = 0;//��ȡ����
        virtual void SetLayout(const BufferLayout& layout)=0;//���ò���
        //
        static Ref<VertexBuffer>  Create(float* vertices, uint32_t size);
    };
//
    class IndexBuffer {
    public:
        virtual ~IndexBuffer(){}
        virtual void Bind()const = 0;//�յ��麯��
        virtual void Unbind() const = 0;

        virtual uint32_t GetCount()const = 0;//���ؼ���
        static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t size);
    };
}