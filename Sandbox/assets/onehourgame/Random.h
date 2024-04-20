#pragma once
//随机数生成器
#include <random>

class Random
{
public:
    static void Init()
    {
        s_RandomEngine.seed(std::random_device()());//初始化或重置随机数生成器的种子
    }

    static float Float()
    {   //得到一个在0到1之间的随机浮点数，那么用这个随机数*任何数x————范围（0——x）
        return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
    }
private:
    static std::mt19937 s_RandomEngine;//高质量的伪随机数生成器
    static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;//均匀整数分布对象
};