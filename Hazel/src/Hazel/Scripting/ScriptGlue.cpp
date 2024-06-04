#include "hzpch.h"
#include "ScriptGlue.h"
#include"ScriptEngine.h"

#include "Hazel/Core/UUID.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Core/Input.h"

#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"

#include "mono/metadata/object.h"

#include "mono/metadata/reflection.h"

#include "box2d/b2_body.h"

#include "Hazel/Physics/Physics2D.h"

namespace Hazel {

    static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;//Entity���͵Ĳ�����������һ������ֵ

    #define HZ_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Hazel.InternalCalls::" #Name, Name)//����ڲ�������(����,��������)

    static void NativeLog(MonoString* string, int parameter)//������־
    {
        char* cStr = mono_string_to_utf8(string);//ת��ΪUTF-8�����C����ַ���
        std::string str(cStr);//ת��Ϊstd::string
        mono_free(cStr);//�ͷ�cStr��ռ�õ��ڴ�
        std::cout << str << ", " << parameter << std::endl;
    }

    static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)//������־ʸ��
    {
        HZ_CORE_WARN("Value: {0}", *parameter);
        *outResult = glm::normalize(*parameter);//parameter��ָ���������һ��
    }

    static float NativeLog_VectorDot(glm::vec3* parameter)//������־_ʸ����
    {
        HZ_CORE_WARN("Value: {0}", *parameter);
        return glm::dot(*parameter, *parameter);//�������������ĵ��
    }
    ///////////////////////////////////////////////////////////////////////////
    static MonoObject* GetScriptInstance(UUID entityID)////ͨ��id��ȡ����
    {
        return ScriptEngine::GetManagedInstance(entityID);
    }

    static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)//ʵ���Ƿ��������Ψһ��ʶ��id��������ͣ�
    {
        //��ȡ��������ʵ��
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        //
        MonoType* managedType = mono_reflection_type_get_type(componentType);//�ҵ���ϣ�е�componentType
        HZ_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
        return s_EntityHasComponentFuncs.at(managedType)(entity);//�� entity ��Ϊ��������[](Entity entity) { return entity.HasComponent<Component>(); }
    }

    static uint64_t Entity_FindEntityByName(MonoString* name)
    {
        char* nameCStr = mono_string_to_utf8(name);

        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->FindEntityByName(nameCStr);//ͨ��name�ҵ�ʵ��
        mono_free(nameCStr);

        if (!entity)
            return 0;

        return entity.GetUUID();//����id
    }

    static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)//��ȡTranslation
    {
        //��ȡ��������ʵ��
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        //
        *outTranslation = entity.GetComponent<TransformComponent>().Translation;//��ȡ��Translation���ݸ�outTranslation����
    }

    static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)//����Translation
    {
        //��ȡ��������ʵ��
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        //
        entity.GetComponent<TransformComponent>().Translation = *translation;//ʵ���Translation����Ϊ��ֵ
    }

    static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)//Ӧ���������壬�ı���������ٶ�
    {
        //��ȡ��������ʵ��
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        //
        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.RuntimeBody;
        body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);//�������������õ��λ��
    }

    static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)//Ӧ���������嵽����
    {
        //��ȡ��������ʵ��
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        //
        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.RuntimeBody;
        body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
    }
    ///
    static void Rigidbody2DComponent_GetLinearVelocity(UUID entityID, glm::vec2* outLinearVelocity)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);

        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.RuntimeBody;
        const b2Vec2& linearVelocity = body->GetLinearVelocity();
        *outLinearVelocity = glm::vec2(linearVelocity.x, linearVelocity.y);
    }

    static Rigidbody2DComponent::BodyType Rigidbody2DComponent_GetType(UUID entityID)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);

        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.RuntimeBody;
        return Utils::Rigidbody2DTypeFromBox2DBody(body->GetType());
    }

    static void Rigidbody2DComponent_SetType(UUID entityID, Rigidbody2DComponent::BodyType bodyType)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);

        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.RuntimeBody;
        body->SetType(Utils::Rigidbody2DTypeToBox2DBody(bodyType));
    }
    ///
    static bool Input_IsKeyDown(KeyCode keycode)
    {
        return Input::IsKeyPressed(keycode);
    }

    ////ע���������ӵ���ϣ��//////////////////////////////////////////////////////////
    template<typename... Component>
    static void RegisterComponent()//������

    {
        ([]()
            {
                std::string_view typeName = typeid(Component).name();//������͵�����
                size_t pos = typeName.find_last_of(':');
                std::string_view structName = typeName.substr(pos + 1);//�����һ��ð��֮��ʼ��ȡ�ַ���
                std::string managedTypename = fmt::format("Hazel.{}", structName);//��ʽ��Ϊ"Hazel.ComponentName"
                //���ĳ���(������c#-��Components.cs)�����Ƿ������Ƶ�����
                MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());//dll���в�������
                if (!managedType)
                {
                    HZ_CORE_ERROR("Could not find component type {}", managedTypename);
                    return;
                }
                s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };//MonoType-���ɵ��ú�������ǰʵ���Ƿ��������
            }(), ...);
    }

    template<typename... Component>
    static void RegisterComponent(ComponentGroup<Component...>)
    {
        RegisterComponent<Component...>();
    }
    void ScriptGlue::RegisterComponents()
    {
        s_EntityHasComponentFuncs.clear();
        RegisterComponent(AllComponents{});//ȫ��������͵�Ԫ��
    }

    ////ע�ắ����������ӵ��ڲ���////////////////////////////////////////////////////////
    void ScriptGlue::RegisterFunctions()
    {
        //print
        HZ_ADD_INTERNAL_CALL(NativeLog);
        HZ_ADD_INTERNAL_CALL(NativeLog_Vector);
        HZ_ADD_INTERNAL_CALL(NativeLog_VectorDot);
        //Component
        HZ_ADD_INTERNAL_CALL(GetScriptInstance);

        HZ_ADD_INTERNAL_CALL(Entity_HasComponent);
        HZ_ADD_INTERNAL_CALL(Entity_FindEntityByName);

        HZ_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
        HZ_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);
        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetLinearVelocity);
        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetType);
        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_SetType);

        HZ_ADD_INTERNAL_CALL(Input_IsKeyDown);
    }

}