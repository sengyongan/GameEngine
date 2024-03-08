workspace"Hazel"
architecture"x64"
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

include "Hazel/vendor/GLFW"
include "Hazel/vendor/Glad"
include "Hazel/vendor/imgui"

project "Hazel"
	location"Hazel"
	kind"SharedLib"--静态库
	language"C++"
	targetdir("bin/"..outputdir.."/%{prj.name}")
	objdir("bin-int/"..outputdir.."/%{prj.name}")
    --在每个文件自动包含pchheader，指明源文件
	pchheader "hzpch.h"
	pchsource "Hazel/src/hzpch.cpp"
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	includedirs--包含目录
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
	}
	links
	{
		"GLFW",
		"Glad",
	"ImGui",
		"opengl32.lib"
	}

	filter"system:windows"
	cppdialect"C++17"
	staticruntime"On"
	systemversion"latest"
	defines
	{
		"HZ_PLATFROM_WINDOWS",
		"HZ_BUILD_DLL",
       -- "HZ_ENABLE_ASSERTS"
       "GLFW_INCLUDE_NONE",
       "HZ_BIND_EVENT_FN"
	}
	postbuildcommands
	{
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
	}
	filter"configurations:Debug"
	defines "HZ_DEBUG"
    buildoptions "/MDd"--多线程调试dll
	symbols"On"

	filter"configurations:Release"
	defines "HZ_RELEASE"
    buildoptions "/MD"
	optimize"On"

	filter"configurations:Dist"
	defines "HZ_DIST"
    buildoptions "/MD"
	optimize"On"

project "Sandbox"
	location"Sandbox"
	kind"ConsoleApp"
	language"C++"
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
		"Hazel/src"
	}

	filter"system:windows"
	cppdialect"C++17"
	staticruntime"On"
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
        buildoptions "/MDd"
	symbols"On"

	filter"configurations:Release"
	defines "HZ_RELEASE"
        buildoptions "/MD"
	optimize"On"

	filter"configurations:Dist"
	defines "HZ_DIST"
        buildoptions "/MD"
	optimize"On"
