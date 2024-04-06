#pragma once//用于确保头文件只被包含一次

#include "Player.h"

struct Pillar//游戏中的障碍物
{
    glm::vec3 TopPosition = { 0.0f, 10.0f, 0.0f };//顶部位置
    glm::vec2 TopScale = { 15.0f, 20.0f };//顶部缩放比例

    glm::vec3 BottomPosition = { 10.0f, 10.0f, 0.0f };//底部位置
    glm::vec2 BottomScale = { 15.0f, 20.0f };//底部缩放比例
};

class Level//表示游戏的关卡
{
public:
    void Init();//初始化关卡

    void OnUpdate(Hazel::Timestep ts);
    void OnRender();//渲染关卡

    void OnImGuiRender();//ImGui库渲染关卡的用户界面

    bool IsGameOver() const { return m_GameOver; }//返回游戏是否结束
    void Reset();//重置关卡

    Player& GetPlayer() { return m_Player; }//返回玩家对象
private:
    void CreatePillar(int index, float offset);//创建一个新的障碍物
    bool CollisionTest();//进行碰撞检测

    void GameOver();//处理游戏结束的逻辑
private:
    Player m_Player;//玩家角色

    bool m_GameOver = false;//游戏是否结束

    float m_PillarTarget = 30.0f;//下一个障碍物的目标位置
    int m_PillarIndex = 0;//当前障碍物的索引
    glm::vec3 m_PillarHSV = { 0.0f, 0.8f, 0.8f };//障碍物的颜色

    std::vector<Pillar> m_Pillars;//所有的障碍物

    Hazel::Ref<Hazel::Texture2D> m_TriangleTexture;//三角纹理
};
