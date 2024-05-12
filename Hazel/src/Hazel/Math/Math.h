#pragma once

#include <glm/glm.hpp>

namespace Hazel::Math {//�����������Ϊ"Hazel"�������ռ��е�"Math"�������ռ�
    //�任������зֽ⣬��ȡ��ƽ�ơ���ת��������Ϣ
    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

}