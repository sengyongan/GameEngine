#pragma once
//�����������
#include <random>

class Random
{
public:
    static void Init()
    {
        s_RandomEngine.seed(std::random_device()());//��ʼ�������������������������
    }

    static float Float()
    {   //�õ�һ����0��1֮����������������ô����������*�κ���x����������Χ��0����x��
        return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
    }
private:
    static std::mt19937 s_RandomEngine;//��������α�����������
    static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;//���������ֲ�����
};