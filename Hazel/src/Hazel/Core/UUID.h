#pragma once
//UUID
#include <xhash>

namespace Hazel {

    class UUID
    {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID&) = default;

        operator uint64_t() const { return m_UUID; }//(uint64_t)����m_UUID
    private:
        uint64_t m_UUID;//��ʶ��
    };

}

namespace std {//std�����ռ�

    template<>
    struct hash<Hazel::UUID>//����ģ��ṹ�壬ΪHazel::UUID��ʵ���������أ���
    {
        std::size_t operator()(const Hazel::UUID& uuid) const
        {
            return hash<uint64_t>()((uint64_t)uuid);//ģ�庯��hash<uint64_t>(���ع�ϣֵ
        }
    };

}