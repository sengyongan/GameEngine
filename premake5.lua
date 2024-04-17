workspace"Hazel"
architecture"x64"
startproject "Sandbox"
configurations--配置调试
{
	"Debug",
	"Release",
	"Dist"
}	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--包含目录（解决方案目录）相对的目录
IncludeDir = {}
IncludeDir["GLFW"] = "Hazel/vendor/GLFW/include"--IncludeDir变量
IncludeDir["Glad"] = "Hazel/vendor/Glad/include"
IncludeDir["ImGui"] = "Hazel/vendor/imgui"
IncludeDir["glm"] = "Hazel/vendor/glm"
IncludeDir["stb_image"] = "Hazel/vendor/stb_image"

include "Hazel/vendor/GLFW"
include "Hazel/vendor/Glad"
include "Hazel/vendor/imgui"
--include "Hazel/vendor/glm"

project "Hazel"
	location"Hazel"
	kind"StaticLib"--静态库
	language"C++"
	cppdialect"C++17"
    staticruntime"on"


	targetdir("bin/"..outputdir.."/%{prj.name}")
	objdir("bin-int/"..outputdir.."/%{prj.name}")
    --在每个文件自动包含pchheader，指明源文件
	pchheader "hzpch.h"
	pchsource "Hazel/src/hzpch.cpp"
	files--添加包含文件（添加到项目）
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
	}
    defines{
        "_CRT_SECURE_NO_WARNINGS"
    }
	includedirs--包含目录
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}
	links
	{
		"GLFW",
		"Glad",
	    "ImGui",
		"opengl32.lib"
	}

	filter"system:windows"
	systemversion"latest"
	defines--定义宏
	{
		"HZ_PLATFROM_WINDOWS",
		"HZ_BUILD_DLL",
       -- "HZ_ENABLE_ASSERTS"--已经被define定义
       "GLFW_INCLUDE_NONE",
       "HZ_BIND_EVENT_FN"
	}
	filter"configurations:Debug"
	defines "HZ_DEBUG"--调试版本
    --buildoptions "/MDd"--多线程调试dll
    runtime "Debug"
	symbols"on"

	filter"configurations:Release"
	defines "HZ_RELEASE"--发布版本
    runtime "Release"
	optimize"on"

	filter"configurations:Dist"
	defines "HZ_DIST"
    runtime "Release"
	optimize"on"

project "Sandbox"
	location"Sandbox"
	kind"ConsoleApp"
	language"C++"
	cppdialect"C++17"
    staticruntime"on"


	targetdir("bin/"..outputdir.."/%{prj.name}")
	objdir("bin-int/"..outputdir.."/%{prj.name}")
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
		
	}
	includedirs
	{
		"Hazel/vendor/spdlog/include",
		"Hazel/src",
		"Hazel/vendor",
        "%{IncludeDir.glm}"
	}

	filter"system:windows"
	systemversion"latest"
	defines
	{
		"HZ_PLATFROM_WINDOWS"
	}
	links
	{
		"Hazel"
	}
	filter"configurations:Debug"
	defines "HZ_DEBUG"
    runtime "Debug"
	symbols"on"

	filter"configurations:Release"
	defines "HZ_RELEASE"
    runtime "Release"
	optimize"on"

	filter"configurations:Dist"
	defines "HZ_DIST"
    runtime "Release"
	optimize"on"


project "Hazel-Editor"
	location"Hazel-Editor"
	kind"ConsoleApp"
	language"C++"
	cppdialect"C++17"
    staticruntime"on"


	targetdir("bin/"..outputdir.."/%{prj.name}")
	objdir("bin-int/"..outputdir.."/%{prj.name}")
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
		
	}
	includedirs
	{
		"Hazel/vendor/spdlog/include",
		"Hazel/src",
		"Hazel/vendor",
        "%{IncludeDir.glm}"
	}

	filter"system:windows"
	systemversion"latest"
	defines
	{
		"HZ_PLATFROM_WINDOWS"
	}
	links
	{
		"Hazel"
	}
	filter"configurations:Debug"
	defines "HZ_DEBUG"
    runtime "Debug"
	symbols"on"

	filter"configurations:Release"
	defines "HZ_RELEASE"
    runtime "Release"
	optimize"on"

	filter"configurations:Dist"
	defines "HZ_DIST"
    runtime "Release"
	optimize"on"
