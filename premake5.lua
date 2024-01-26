workspace"Hazel"
architecture"x64"
configurations
{
	"Debug",
	"Release",
	"Dist"
}	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--包含目录（解决方案目录）相对的目录
IncludeDir={}
IncludeDir["GLFW"]="Hazel/vendor/GLFW/include"--IncludeDir变量
include"Hazel/vendor/GLFW"

project "Hazel"
	location"Hazel"
	kind"SharedLib"--静态库
	language"C++"
	targetdir("bin/"..outputdir.."/%{prj.name}")
	objdir("bin-int/"..outputdir.."/%{prj.name}")

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
		"{IncludeDir.GLFW}"
	}
	links
	{
		"GLFW",
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
	}
	postbuildcommands
	{
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
	}
	filter"configurations:Debug"
	defines "HZ_DEBUG"
	symbols"On"

	filter"configurations:Release"
	defines "HZ_RELEASE"
	optimize"On"

	filter"configurations:Dist"
	defines "HZ_DIST"
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
	symbols"On"

	filter"configurations:Release"
	defines "HZ_RELEASE"
	optimize"On"

	filter"configurations:Dist"
	defines "HZ_DIST"
	optimize"On"
