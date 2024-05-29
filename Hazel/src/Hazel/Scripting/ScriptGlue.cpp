#include "hzpch.h"
#include "ScriptGlue.h"

#include "mono/metadata/object.h"

namespace Hazel {

    #define HZ_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Hazel.InternalCalls::" #Name, Name)//添加内部调用中(名称,本机方法)

    static void NativeLog(MonoString* string, int parameter)//本地日志
    {
        char* cStr = mono_string_to_utf8(string);//转换为UTF-8编码的C风格字符串
        std::string str(cStr);//转换为std::string
        mono_free(cStr);//释放cStr所占用的内存
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

    void ScriptGlue::RegisterFunctions()
    {
        HZ_ADD_INTERNAL_CALL(NativeLog);
        HZ_ADD_INTERNAL_CALL(NativeLog_Vector);
        HZ_ADD_INTERNAL_CALL(NativeLog_VectorDot);
    }

}