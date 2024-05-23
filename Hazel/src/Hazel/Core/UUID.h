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

        operator uint64_t() const { return m_UUID; }//(uint64_t)返回m_UUID
    private:
        uint64_t m_UUID;//标识符
    };

}

namespace std {

    template<>
    struct hash<Hazel::UUID>
    {
        std::size_t operator()(const Hazel::UUID& uuid) const
        {
            return hash<uint64_t>()((uint64_t)uuid);//模板函数hash<uint64_t>(返回哈希值
        }
    };

}