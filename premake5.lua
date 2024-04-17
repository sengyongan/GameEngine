workspace"Hazel"
architecture"x64"
startproject "Sandbox"
configurations--���õ���
{
	"Debug",
	"Release",
	"Dist"
}	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--����Ŀ¼���������Ŀ¼����Ե�Ŀ¼
IncludeDir = {}
IncludeDir["GLFW"] = "Hazel/vendor/GLFW/include"--IncludeDir����
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
	kind"StaticLib"--��̬��
	language"C++"
	cppdialect"C++17"
    staticruntime"on"


	targetdir("bin/"..outputdir.."/%{prj.name}")
	objdir("bin-int/"..outputdir.."/%{prj.name}")
    --��ÿ���ļ��Զ�����pchheader��ָ��Դ�ļ�
	pchheader "hzpch.h"
	pchsource "Hazel/src/hzpch.cpp"
	files--��Ӱ����ļ�����ӵ���Ŀ��
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
	includedirs--����Ŀ¼
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
	defines--�����
	{
		"HZ_PLATFROM_WINDOWS",
		"HZ_BUILD_DLL",
       -- "HZ_ENABLE_ASSERTS"--�Ѿ���define����
       "GLFW_INCLUDE_NONE",
       "HZ_BIND_EVENT_FN"
	}
	filter"configurations:Debug"
	defines "HZ_DEBUG"--���԰汾
    --buildoptions "/MDd"--���̵߳���dll
    runtime "Debug"
	symbols"on"

	filter"configurations:Release"
	defines "HZ_RELEASE"--�����汾
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
