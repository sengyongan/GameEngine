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

    namespace Utils {

        std::string MonoStringToString(MonoString* string)//MonoString转化为string
        {
            char* cStr = mono_string_to_utf8(string);
            std::string str(cStr);
            mono_free(cStr);
            return str;
        }

    }

    static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;//Entity类型的参数，并返回一个布尔值

    #define HZ_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Hazel.InternalCalls::" #Name, Name)//添加内部调用中(名称,本机方法)

    static void NativeLog(MonoString* string, int parameter)//本地日志
    {
        std::string str = Utils::MonoStringToString(string);
        std::cout << str << ", " << parameter << std::endl;
    }

    static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)//本地日志矢量
    {
        HZ_CORE_WARN("Value: {0}", *parameter);
        *outResult = glm::normalize(*parameter);//parameter所指向的向量归一化
    }

    static float NativeLog_VectorDot(glm::vec3* parameter)//本地日志_矢量点
    {
        HZ_CORE_WARN("Value: {0}", *parameter);
        return glm::dot(*parameter, *parameter);//计算两个向量的点积
    }
    ///////////////////////////////////////////////////////////////////////////
    static MonoObject* GetScriptInstance(UUID entityID)////通过id获取基类
    {
        return ScriptEngine::GetManagedInstance(entityID);
    }

    static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)//实体是否有组件（唯一标识符id，组件类型）
    {
        //获取到场景和实体
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        //
        MonoType* managedType = mono_reflection_type_get_type(componentType);//找到哈希中的componentType
        HZ_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
        return s_EntityHasComponentFuncs.at(managedType)(entity);//将 entity 作为参数传递[](Entity entity) { return entity.HasComponent<Component>(); }
    }

    static uint64_t Entity_FindEntityByName(MonoString* name)
    {
        char* nameCStr = mono_string_to_utf8(name);

        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->FindEntityByName(nameCStr);//通过name找到实体
        mono_free(nameCStr);

        if (!entity)
            return 0;

        return entity.GetUUID();//返回id
    }

    static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)//获取Translation
    {
        //获取到场景和实体
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        //
        *outTranslation = entity.GetComponent<TransformComponent>().Translation;//获取到Translation传递给outTranslation参数
    }

    static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)//设置Translation
    {
        //获取到场景和实体
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        //
        entity.GetComponent<TransformComponent>().Translation = *translation;//实体的Translation设置为新值
    }

    static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)//应用线性脉冲，改变物体的线速度
    {
        //获取到场景和实体
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        //
        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.RuntimeBody;
        body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);//冲量向量，作用点的位置
    }

    static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)//应用线性脉冲到中心
    {
        //获取到场景和实体
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
    ////////////////////////////////////////////////////////////////////////
    static MonoString* TextComponent_GetText(UUID entityID)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

        auto& tc = entity.GetComponent<TextComponent>();
        return ScriptEngine::CreateString(tc.TextString.c_str());
    }

    static void TextComponent_SetText(UUID entityID, MonoString* textString)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

        auto& tc = entity.GetComponent<TextComponent>();
        tc.TextString = Utils::MonoStringToString(textString);
    }

    static void TextComponent_GetColor(UUID entityID, glm::vec4* color)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

        auto& tc = entity.GetComponent<TextComponent>();
        *color = tc.Color;
    }

    static void TextComponent_SetColor(UUID entityID, glm::vec4* color)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

        auto& tc = entity.GetComponent<TextComponent>();
        tc.Color = *color;
    }

    static float TextComponent_GetKerning(UUID entityID)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

        auto& tc = entity.GetComponent<TextComponent>();
        return tc.Kerning;
    }

    static void TextComponent_SetKerning(UUID entityID, float kerning)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

        auto& tc = entity.GetComponent<TextComponent>();
        tc.Kerning = kerning;
    }

    static float TextComponent_GetLineSpacing(UUID entityID)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

        auto& tc = entity.GetComponent<TextComponent>();
        return tc.LineSpacing;
    }

    static void TextComponent_SetLineSpacing(UUID entityID, float lineSpacing)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        HZ_CORE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        HZ_CORE_ASSERT(entity);
        HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

        auto& tc = entity.GetComponent<TextComponent>();
        tc.LineSpacing = lineSpacing;
    }

    /////////////////////////////////////////////////////////////////////////
    static bool Input_IsKeyDown(KeyCode keycode)
    {
        return Input::IsKeyPressed(keycode);
    }

    ////注册组件（添加到哈希）//////////////////////////////////////////////////////////
    template<typename... Component>
    static void RegisterComponent()//添加组件

    {
        ([]()
            {
                std::string_view typeName = typeid(Component).name();//组件类型的名称
                size_t pos = typeName.find_last_of(':');
                std::string_view structName = typeName.substr(pos + 1);//从最后一个冒号之后开始截取字符串
                std::string managedTypename = fmt::format("Hazel.{}", structName);//格式化为"Hazel.ComponentName"
                //核心程序集(定义在c#-》Components.cs)查找是否有名称的类型
                MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());//dll库中查找类型
                if (!managedType)
                {
                    HZ_CORE_ERROR("Could not find component type {}", managedTypename);
                    return;
                }
                s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };//MonoType-》可调用函数（当前实体是否有组件）
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
        RegisterComponent(AllComponents{});//全部组件类型的元组
    }

    ////注册函数（函数添加到内部）////////////////////////////////////////////////////////
    void ScriptGlue::RegisterFunctions()
    {
        //print
        HZ_ADD_INTERNAL_CALL(NativeLog);
        HZ_ADD_INTERNAL_CALL(NativeLog_Vector);
        HZ_ADD_INTERNAL_CALL(NativeLog_VectorDot);
        //Component
        HZ_ADD_INTERNAL_CALL(GetScriptInstance);
        //
        HZ_ADD_INTERNAL_CALL(Entity_HasComponent);
        HZ_ADD_INTERNAL_CALL(Entity_FindEntityByName);
        //
        HZ_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
        HZ_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
        //
        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);
        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetLinearVelocity);
        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetType);
        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_SetType);
        //
        HZ_ADD_INTERNAL_CALL(TextComponent_GetText);
        HZ_ADD_INTERNAL_CALL(TextComponent_SetText);
        HZ_ADD_INTERNAL_CALL(TextComponent_GetColor);
        HZ_ADD_INTERNAL_CALL(TextComponent_SetColor);
        HZ_ADD_INTERNAL_CALL(TextComponent_GetKerning);
        HZ_ADD_INTERNAL_CALL(TextComponent_SetKerning);
        HZ_ADD_INTERNAL_CALL(TextComponent_GetLineSpacing);
        HZ_ADD_INTERNAL_CALL(TextComponent_SetLineSpacing);
        //
        HZ_ADD_INTERNAL_CALL(Input_IsKeyDown);
    }

}