#include "hzpch.h"
#include "ScriptGlue.h"

#include "mono/metadata/object.h"

namespace Hazel {

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

    void ScriptGlue::RegisterFunctions()
    {
        HZ_ADD_INTERNAL_CALL(NativeLog);
        HZ_ADD_INTERNAL_CALL(NativeLog_Vector);
        HZ_ADD_INTERNAL_CALL(NativeLog_VectorDot);
    }

}