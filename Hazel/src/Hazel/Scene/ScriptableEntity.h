#pragma once
//为实体添加的脚本，新脚本的管理
#include"Entity.h"

namespace Hazel {
    class ScriptableEntity {
    public:
        virtual ~ScriptableEntity() {};

        template <typename T>
        T& GetComponent() {//获取组件
            return m_Entity.GetComponent<T>();
        }

    protected:
        virtual void OnCreate() {};
        virtual void OnDestory() {};
        virtual void OnUpdate(Timestep ts) {};

    private:
        Entity m_Entity;
        friend class Scene;
    };
}