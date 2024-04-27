#pragma once
//Ϊʵ����ӵĽű����½ű��Ĺ���
#include"Entity.h"

namespace Hazel {
    class ScriptableEntity {
    public:
        virtual ~ScriptableEntity() {};

        template <typename T>
        T& GetComponent() {//��ȡ���
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