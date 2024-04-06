#pragma once
//预编译头
//platfrom
#include <iostream>
#include <memory>//使用头指针
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>//使用<<
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include"Hazel/Core/Log.h"//日志
#include"Hazel/Debug/Instrumentor.h"//测量代码段的执行时间

#ifdef HZ_PLATFROM_WINDOWS
#include <Windows.h>
#endif
