#pragma once
#include"entt.hpp"
namespace Hazel {
    class Scene {
    public:
        Scene();
        ~Scene();
    private:
        entt::registry m_Registry;//用于实体管理和组件管理
    };
}