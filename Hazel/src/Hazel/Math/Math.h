#pragma once

#include <glm/glm.hpp>

namespace Hazel::Math {//代码块属于名为"Hazel"的命名空间中的"Math"子命名空间
    //变换矩阵进行分解，提取出平移、旋转和缩放信息
    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

}