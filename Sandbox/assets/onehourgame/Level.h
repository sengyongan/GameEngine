#pragma once//����ȷ��ͷ�ļ�ֻ������һ��

#include "Player.h"

struct Pillar//��Ϸ�е��ϰ���
{
    glm::vec3 TopPosition = { 0.0f, 10.0f, 0.0f };//����λ��
    glm::vec2 TopScale = { 15.0f, 20.0f };//�������ű���

    glm::vec3 BottomPosition = { 10.0f, 10.0f, 0.0f };//�ײ�λ��
    glm::vec2 BottomScale = { 15.0f, 20.0f };//�ײ����ű���
};

class Level//��ʾ��Ϸ�Ĺؿ�
{
public:
    void Init();//��ʼ���ؿ�

    void OnUpdate(Hazel::Timestep ts);
    void OnRender();//��Ⱦ�ؿ�

    void OnImGuiRender();//ImGui����Ⱦ�ؿ����û�����

    bool IsGameOver() const { return m_GameOver; }//������Ϸ�Ƿ����
    void Reset();//���ùؿ�

    Player& GetPlayer() { return m_Player; }//������Ҷ���
private:
    void CreatePillar(int index, float offset);//����һ���µ��ϰ���
    bool CollisionTest();//������ײ���

    void GameOver();//������Ϸ�������߼�
private:
    Player m_Player;//��ҽ�ɫ

    bool m_GameOver = false;//��Ϸ�Ƿ����

    float m_PillarTarget = 30.0f;//��һ���ϰ����Ŀ��λ��
    int m_PillarIndex = 0;//��ǰ�ϰ��������
    glm::vec3 m_PillarHSV = { 0.0f, 0.8f, 0.8f };//�ϰ������ɫ

    std::vector<Pillar> m_Pillars;//���е��ϰ���

    Hazel::Ref<Hazel::Texture2D> m_TriangleTexture;//��������
};
