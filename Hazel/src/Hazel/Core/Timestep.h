#pragma once
//时差
namespace Hazel {
    class Timestep {
    public:
        Timestep(float time = 0.0f)
            :m_Time(time) {//m_Time初始为0

        }
        operator float() const { return m_Time; }//重载类型转换
        float GetSeconds() const { return m_Time; }//获取时差
        float GetMilliseconds() const { return m_Time * 1000.0f; }//获取毫秒
    private:
        float m_Time;//时差
    };
}